/*
    This file is part of Ample.

    Ample is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ample is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ample.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "interpreter.h"
#include "bool.h"
#include "array.h"
#include "dict_vars.h"
#include "objects/ampobject.h"
#include "objects/numobject.h"
#include "objects/boolobject.h"
#include "objects/strobject.h"
#include "interpreter_functions.h"

#include <assert.h>
#include <string.h>
static DICT(ObjVars) global_variables;
DICT_DECLARE (Func, const char *, ASTHandle);
DICT_IMPL (Func, const char *, ASTHandle)
static DICT(Func) func_dict;

void
interpreter_erase_variable_if_exists (const char *var,
                                      DICT (ObjVars) *local_variables)
{

  AmpObject *obj = NULL;
  bool32 success = DictObjVars_get_and_erase (local_variables, var, &obj);
  if (success)
    AmpObjectDecrementRefcount (obj);
}

void
interpreter_add_obj_mapping (const char *var_name,
                             AmpObject *obj,
                             DICT (ObjVars) *local_variables)
{
  interpreter_erase_variable_if_exists (var_name, local_variables);
  DictObjVars_insert (local_variables, var_name, obj);
}

void
InterpreterStart (ASTHandle head)
{
  DictFunc_init (&func_dict, hash_string, string_compare, 10);
  /* evaluate the global scope */
  interpreter_evaluate_scope (head, NULL, false);
  DictFunc_free (&func_dict);
}

AmpObject *
interpreter_evaluate_statement (ASTHandle statement,
                                DICT (ObjVars) **variable_scope_stack)
{
  struct AST *s = ast_get_node (statement);
  if (s->type == AST_ASSIGNMENT)
    {
      interpreter_evaluate_assignment (statement, variable_scope_stack);
    }
  else if (s->type == AST_BINARY_OP)
    {
      return interpreter_evaluate_binary_op (statement, variable_scope_stack);
    }
  else if (s->type == AST_IF)
    {
      interpreter_evaluate_if (statement, variable_scope_stack);
    }
  else if (s->type == AST_EQUALITY)
    {
      return interpreter_evaluate_equality (statement, variable_scope_stack);
    }
  else if (s->type == AST_FUNC)
    {
      interpreter_insert_function_into_dict (statement); 
    }
  else if (s->type == AST_FUNC_CALL)
    {
      return interpreter_evaluate_function_call (statement, variable_scope_stack);
    }
  return NULL;
}

DICT (ObjVars) **
interpreter_create_new_variable_scope_stack (DICT (ObjVars) *local_variables,
                                             DICT (ObjVars) **var_scope_stack)
{
  size_t i;
  DICT (ObjVars) **new_variable_scope_stack = NULL;
  /* the local variables will be at index 0 */
  ARRAY_PUSH (new_variable_scope_stack, local_variables);
  if (var_scope_stack)
    {
      size_t arr_size = ARRAY_COUNT (var_scope_stack);
      for (i = 0; i < arr_size; i++)
        {
          ARRAY_PUSH (new_variable_scope_stack, var_scope_stack[i]); 
        }
    }

  return new_variable_scope_stack;
}

void
interpreter_free_local_variables (DICT (ObjVars) *local_variables)
{
  size_t i;
  for (i = 1; i < ARRAY_COUNT (local_variables->mem); i++)
    {
      AmpObjectDecrementRefcount (local_variables->mem[i].val);
    }
  DictObjVars_free (local_variables);
  free (local_variables);
}

AmpObject *
interpreter_evaluate_function_call (ASTHandle func_call,
                                    DICT (ObjVars) **variable_scope_stack)
{
  ASTHandle func_handle;
  bool32 user_defined_function;
  struct AST *func_call_node = ast_get_node (func_call);
  const char *func_name = func_call_node->d.func_call_data.name;

  /* try to find the func definition */
  user_defined_function = DictFunc_get (&func_dict,
                                               func_name,
                                               &func_handle);
  if (user_defined_function)
    {
      /* execute a user defined function */
      /* copy args to a local scope */
      DICT(ObjVars) *local_variables = malloc (sizeof (DICT (ObjVars)));
      DICT (ObjVars) **new_variable_scope_stack = NULL;
      struct AST *func_node = ast_get_node (func_handle);
      ASTHandle *args = func_node->d.func_data.args;
      ASTHandle *args_input = func_call_node->d.func_call_data.args;
      size_t arg_count = ARRAY_COUNT (args);
      size_t arg_input_count = ARRAY_COUNT (args_input);
      size_t i;
      DictObjVars_init (local_variables, hash_string, string_compare, 10);

      if (arg_count != arg_input_count)
        {
          printf ("Invalid number of arguments for function \"%s\",",
                  func_name);
          printf ("expected %u arguments and %u were provided\n",
                  (unsigned int) arg_count,
                  (unsigned int) arg_input_count);
          exit (1);
        }
      for (i = 0; i < arg_count; i++)
        {
          struct AST *arg = ast_get_node (args[i]);
          if (arg->type == AST_IDENTIFIER)
            {
              AmpObject *obj =
                InterpreterGetOrGenerateAmpObject (args_input[i],
                                                   variable_scope_stack);
              DictObjVars_insert (local_variables, arg->d.id_data.id, obj);
            }
          else
            {
              printf ("Function arg specifiers should be identifier\n");
              exit (1);
            }
        }
      new_variable_scope_stack =
        interpreter_create_new_variable_scope_stack (local_variables,
                                                     variable_scope_stack);

      /* this will free the local scope upon finishing */
      return interpreter_evaluate_scope (func_node->d.func_data.scope,
                                         new_variable_scope_stack,
                                         true);
    }
  else
    {
      ASTHandle *args_input = func_call_node->d.func_call_data.args;
      size_t arg_count = ARRAY_COUNT (args_input);
      AmpObject *obj;
      if (!ExecuteAmpleFunction (args_input,
                                 arg_count,
                                 func_name,
                                 variable_scope_stack,
                                 &obj))
        {
          printf ("Function does not exist: %s\n",
                  func_call_node->d.func_call_data.name);
          exit (1);
        }
      if (obj)
        return obj;
    }
  return NULL;
}

void
interpreter_insert_function_into_dict (ASTHandle func_handle)
{
  const char *func_name = ast_get_node (func_handle)->d.func_data.name;
  DictFunc_insert (&func_dict, func_name, func_handle);
}

AmpObject *
interpreter_evaluate_equality (ASTHandle equality_handle,
                               DICT (ObjVars) **variable_scope_stack)
{
  struct AST *equality_ast = ast_get_node (equality_handle);
  if (equality_ast->type == AST_EQUALITY)
    {
      /* get amp objects to work with */
      AmpObject *left_obj =
        InterpreterGetOrGenerateAmpObject
          (equality_ast->d.equality_data.left,
           variable_scope_stack);
      AmpObject *right_obj =
        InterpreterGetOrGenerateAmpObject
          (equality_ast->d.equality_data.right,
           variable_scope_stack);
      AmpObject *retval = NULL;


      if (left_obj->info->type == right_obj->info->type)
        {
          retval = left_obj->info->ops.equal (left_obj, right_obj);
        }
      else
        {
          printf ("Cannot perform equality operation on different types\n");
          exit (1);
        }
      AmpObjectDecrementRefcount (left_obj);
      AmpObjectDecrementRefcount (right_obj);
      return retval;
    }
  else
    {
      printf ("Statement is not an equality statement\n");
      exit (1);
    }
}

AmpObject *
interpreter_evaluate_statement_to_bool32 (ASTHandle statement_handle,
                                          DICT (ObjVars) **variable_scope_stack)
{
  struct AST *expr = ast_get_node (statement_handle);
  if (expr->type == AST_BOOL)
    {
      return AmpBoolCreate (expr->d.bool_data.value);
    }
  else if (expr->type == AST_IDENTIFIER)
    {
      /* assume we are given a pre-existing variable */
      char *identifier_str = expr->d.id_data.id;
      AmpObject *obj = interpreter_get_amp_object (identifier_str,
                                                   variable_scope_stack);
      if (obj->info->type == AMP_OBJ_BOOL)
        {
          AmpObjectIncrementRefcount (obj);
          return obj;
        }
      else
        {
          printf ("Variable \"%s\" is not of type bool\n", identifier_str);
          exit (1);
        }
    }
  else if (expr->type == AST_EQUALITY)
    {
      AmpObject *obj = interpreter_evaluate_equality (statement_handle,
                                                      variable_scope_stack);
      return obj;
    }
  else
    {
      printf ("Expression does not evaluate to a bool\n");
      exit (1);
    }
}

AmpObject *
interpreter_evaluate_scope (ASTHandle scope_handle,
                            DICT (ObjVars) **variable_scope_stack,
                            bool32 local_scope_already_created)
{
  struct AST *scope = ast_get_node (scope_handle);
  if (!scope)
    { 
      if (local_scope_already_created)
        {
          interpreter_free_local_variables (variable_scope_stack[0]);
          ARRAY_FREE (variable_scope_stack);
        }
      return NULL;
    }
  /* set up the scope's variable stacks */
  DICT(ObjVars) **new_variable_scope_stack = NULL;
  DICT(ObjVars) *local_variables;
  size_t statement_count;
  size_t i;

  if (!local_scope_already_created)
    {
      local_variables = malloc (sizeof(DICT(ObjVars)));
      DictObjVars_init (local_variables, hash_string, string_compare, 10);
      new_variable_scope_stack = 
        interpreter_create_new_variable_scope_stack (local_variables,
                                                     variable_scope_stack);
    }
  else
    {
      new_variable_scope_stack = variable_scope_stack;
    }

  statement_count = ARRAY_COUNT (scope->d.scope_data.statements);
  for (i = 0; i < statement_count; i++)
    {
      AmpObject *obj;
      obj = interpreter_evaluate_statement (scope->d.scope_data.statements[i],
                                            new_variable_scope_stack);
      if (obj)
        {
          /* NOTE: placeholder */
          AmpObjectDecrementRefcount (obj);
          /* if we get a return amp object then return something that
           * is not null */
        }
    }

#ifdef INTERPRETER_DEBUG
  debug__interpreter_print_all_vars (new_variable_scope_stack[0]);
#endif
  interpreter_free_local_variables (new_variable_scope_stack[0]);
  ARRAY_FREE (new_variable_scope_stack);

  return NULL;
}

void
interpreter_evaluate_if (ASTHandle statement,
                         DICT (ObjVars) **variable_scope_stack)
{
  struct AST *if_node = ast_get_node (statement);
  if (if_node->type == AST_IF)
    {
      /* run different scopes depending on the if's true or false */
      struct AST *expr_node = ast_get_node (statement);
      AmpObject *is_expr_true;

      /* expr_node should evaluate to a bool32 */
      is_expr_true = 
        interpreter_evaluate_statement_to_bool32 (expr_node->d.if_data.expr,
                                                  variable_scope_stack);

      if (AMP_BOOL (is_expr_true)->val)
        interpreter_evaluate_scope (expr_node->d.if_data.scope_if_true,
                                    variable_scope_stack,
                                    false);
      else if (AMP_BOOL(is_expr_true)->val == false &&
               expr_node->d.if_data.scope_if_false)
        interpreter_evaluate_scope (expr_node->d.if_data.scope_if_false,
                                    variable_scope_stack,
                                    false);


      AmpObjectDecrementRefcount (is_expr_true);
    }
}

AmpObject *
interpreter_get_amp_object (const char *var,
                            DICT (ObjVars) **variable_scope_stack)
{
  AmpObject *obj = NULL;
  bool32 success = false;
  size_t i;
  for (i = 0; i < ARRAY_COUNT (variable_scope_stack); i++)
    {
      success = DictObjVars_get (variable_scope_stack[i], var, &obj);
      if (success)
        break;
    }
  if (!success)
    {
      printf ("Variable \"%s\" does not exist\n", var);
      exit (1);
    }
  return obj;
}

/* returns an owning pointer to an Amp Object
   i.e the object returned will have it's reference counter incremented */
AmpObject *
InterpreterGetOrGenerateAmpObject (ASTHandle handle,
                                   DICT (ObjVars) **__restrict__ variable_scope_stack)
{
  struct AST *node = ast_get_node (handle);
  AmpObject *obj = NULL;
  switch (node->type)
    {
    case AST_IDENTIFIER:
      obj = interpreter_get_amp_object (node->d.id_data.id,
                                        variable_scope_stack);
      AmpObjectIncrementRefcount (obj);
      break;
    case AST_INTEGER:
      obj = AmpNumberCreate (node->d.int_data.value);
      break;
    case AST_STRING:
      obj = AmpStringCreate (node->d.str_data.str);
      break;
    case AST_BINARY_OP:
      obj = interpreter_evaluate_binary_op (handle, variable_scope_stack);
      break;
    default:
      assert (false);
      break;
    }
  return obj;
}

AmpObject *
interpreter_evaluate_binary_op (ASTHandle handle,
                                DICT (ObjVars) **variable_scope_stack)
{
  struct AST *node = ast_get_node (handle);

  if (node->type == AST_BINARY_OP)
    {
      ASTHandle right_handle = node->d.bop_data.right;
      ASTHandle left_handle = node->d.bop_data.left;
      struct AST *right_node = ast_get_node (right_handle);
      struct AST *left_node = ast_get_node (left_handle);
      AmpObject *left = NULL, *right = NULL;

      if (right_node->type != AST_BINARY_OP)
        {
          right = InterpreterGetOrGenerateAmpObject (right_handle,
                                                          variable_scope_stack);
        }
      else if (right_node->type == AST_BINARY_OP)
        {
          right = interpreter_evaluate_binary_op (right_handle,
                                                  variable_scope_stack);
        }
      else
        {
          exit (1);
        }

      if (left_node->type != AST_BINARY_OP)
        {
          left = InterpreterGetOrGenerateAmpObject (left_handle,
                                                         variable_scope_stack);
        }
      else if (left_node->type == AST_BINARY_OP)
        {
          left = interpreter_evaluate_binary_op (left_handle,
                                                 variable_scope_stack);
        }
      else
        {
          exit (1);
        }

      if (left->info->type == right->info->type)
        {
          AmpObject *obj = NULL;

          switch (node->d.bop_data.op)
            {
              case '+': obj = right->info->ops.add (right, left);
                        break;
              case '-': obj = right->info->ops.sub (right, left);
                        break;
              case '*': obj = right->info->ops.mult (right, left);
                        break;
              case '/': obj = right->info->ops.div (right, left);
                        break;
              default: printf ("Invalid binary operation\n");
                       exit (1);
            }
          AmpObjectDecrementRefcount (left);
          AmpObjectDecrementRefcount (right);
          return obj;
        }
      else
        {
          printf ("Attempting binary operation on values of different types\n");
          exit (1);
        }
    }
  return 0;
}

void
interpreter_duplicate_variable (const char *var,
                                const char *assign,
                                DICT (ObjVars) **variable_scope_stack,
                                size_t scope_stack_index)
{
  AmpObject *obj = interpreter_get_amp_object (var, variable_scope_stack);
  DICT (ObjVars) *local_vars = variable_scope_stack[scope_stack_index];
  /* new variable will be referencing the same memory */
  AmpObjectIncrementRefcount (obj);
  interpreter_add_obj_mapping (assign, obj, local_vars);
}

void
interpreter_evaluate_assignment (ASTHandle statement,
                                 DICT (ObjVars) **variable_scope_stack)
{
  struct AST *s = ast_get_node (statement);
  struct AST *expr = ast_get_node (s->d.asgn_data.expr);
  size_t i;
  AmpObject *parent_obj = NULL;
  bool32 exists_in_parent_scope = false;
  const char *var = s->d.asgn_data.var;
  size_t scope_stack_index = 0;
  /* make sure that the variable doesn't already exist in a parent scope */
  for (i = 1; i < ARRAY_COUNT (variable_scope_stack); i++)
    {
      exists_in_parent_scope = DictObjVars_get (variable_scope_stack[i],
                                                var,
                                                &parent_obj);
      if (exists_in_parent_scope)
        {
          scope_stack_index = i;
          break;
        }
    }
  if (expr->type == AST_INTEGER)
    {
      double val = expr->d.int_data.value;
      AmpObject *obj = AmpNumberCreate (val);
      interpreter_add_obj_mapping (var,
                                   obj,
                                   variable_scope_stack[scope_stack_index]);
    }
  else if (expr->type == AST_BINARY_OP)
    {
      AmpObject *obj = interpreter_evaluate_binary_op (s->d.asgn_data.expr,
                                                       variable_scope_stack);
      interpreter_add_obj_mapping (var,
                                   obj,
                                   variable_scope_stack[scope_stack_index]);
    }
  else if (expr->type == AST_STRING)
    {
      const char *val = expr->d.str_data.str;
      AmpObject *obj = AmpStringCreate (val);
      interpreter_add_obj_mapping (var,
                                   obj,
                                   variable_scope_stack[scope_stack_index]);
    }
  else if (expr->type == AST_BOOL)
    {
      bool32 val = expr->d.bool_data.value;
      AmpObject *obj = AmpBoolCreate (val);
      interpreter_add_obj_mapping (var,
                                   obj,
                                   variable_scope_stack[scope_stack_index]);
    }
  else if (expr->type == AST_IDENTIFIER)
    {
      const char *expr_var = expr->d.id_data.id;
      interpreter_duplicate_variable (expr_var,
                                      var,
                                      variable_scope_stack,
                                      scope_stack_index);
    }
  else if (expr->type == AST_FUNC_CALL)
    {
      AmpObject *obj = interpreter_evaluate_function_call (s->d.asgn_data.expr,
                                                           variable_scope_stack);
      interpreter_add_obj_mapping (var,
                                   obj,
                                   variable_scope_stack[scope_stack_index]);
    }
}

void
debug__interpreter_print_all_vars (DICT (ObjVars) *vars)
{
  size_t i;
  printf ("DEBUG output of variable map...\n");
  for (i = 0; i < vars->capacity; i++)
    {
      DictEntryHandle h = vars->map[i];
      if (h != 0)
        {
          const DICT_ENTRY (ObjVars) *e = &vars->mem[h];
          AmpObject *obj = e->val;
          switch (obj->info->type)
            {
            case AMP_OBJ_INT:
              printf ("Int Variable: %s\n\tValue: %f\n",
                      e->key, AMP_NUMBER (obj)->val);
              break;
            case AMP_OBJ_STR:
              printf ("Str Variable: %s\n\tValue: %s\n",
                      e->key, AMP_STRING (obj)->string);
              break;
            case AMP_OBJ_BOOL:
              printf ("Bool Variable %s\n\tValue: %s\n", e->key, 
                      AMP_BOOL (obj)->val ? "true" : "false");
              break;
            }
        }
    }
}

AmpObject *
debug__interpreter_get_variable_object (const char *var)
{
  AmpObject *obj = NULL;
  DictObjVars_get (&global_variables, var, &obj);
  return obj;
}
