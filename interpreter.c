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
#include "ample_errors.h"
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
#define DEFUALT_DICT_INIT_COUNT 1
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
  bool32 should_return = false;
  interpreter_evaluate_scope (head, NULL, false, &should_return);
  DictFunc_free (&func_dict);
}

AmpObject *
interpreter_evaluate_statement (ASTHandle statement,
                                DICT (ObjVars) **variable_scope_stack,
                                bool32 *return_from_scope)
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
      return interpreter_evaluate_if (statement, variable_scope_stack, return_from_scope);
    }
  else if (s->type == AST_BINARY_COMPARATOR)
    {
      return interpreter_evaluate_binary_comparison (statement, variable_scope_stack);
    }
  else if (s->type == AST_FUNC)
    {
      interpreter_insert_function_into_dict (statement); 
    }
  else if (s->type == AST_FUNC_CALL)
    {
      return interpreter_evaluate_function_call (statement,
                                                 variable_scope_stack,
                                                 return_from_scope);
    }

  switch (s->type)
    {
    case AST_ASSIGNMENT:
      interpreter_evaluate_assignment (statement, variable_scope_stack);
      break;
    case AST_BINARY_OP:
      return interpreter_evaluate_binary_op (statement, variable_scope_stack);
      break;
    case AST_IF:
      return interpreter_evaluate_if (statement,
                                      variable_scope_stack,
                                      return_from_scope);
      break;
    case AST_BINARY_COMPARATOR:
      return interpreter_evaluate_binary_comparison (statement,
                                                     variable_scope_stack);
      break;
    case AST_FUNC:
      interpreter_insert_function_into_dict (statement);
      break;
    case AST_FUNC_CALL:
      return interpreter_evaluate_function_call (statement,
                                                 variable_scope_stack,
                                                 return_from_scope);
      break;
    case AST_INTEGER:
      return AmpNumberCreate (s->d.int_data.value);
      break;
    case AST_STRING:
      return AmpStringCreate (s->d.str_data.str);
      break;
    case AST_BOOL:
      return AmpBoolCreate (s->d.bool_data.value);
      break;
    case AST_IDENTIFIER:
      return interpreter_find_variable (s->d.id_data.id,
                                        variable_scope_stack);
      break;

    }
  return NULL;
}

AmpObject *
interpreter_find_variable (const char *var,
                           DICT (ObjVars) **variable_scope_stack)
{
  /* make sure that the variable doesn't already exist in a parent scope */
  size_t i = 0;
  AmpObject *obj = NULL;
  for (i = 0; i < ARRAY_COUNT (variable_scope_stack); i++)
    {
      bool32 exists_in_scope = DictObjVars_get (variable_scope_stack[i],
                                                var,
                                                &obj);
      if (exists_in_scope)
        return obj;
    }
  return obj;
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
                                    DICT (ObjVars) **variable_scope_stack,
                                    bool32 *return_from_scope)
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
      DictObjVars_init (local_variables,
                        hash_string,
                        string_compare,
                        DEFUALT_DICT_INIT_COUNT);

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
      bool32 should_return = false;
      if (return_from_scope)
        return interpreter_evaluate_scope (func_node->d.func_data.scope,
                                           new_variable_scope_stack,
                                           true,
                                           return_from_scope);
      else
        return interpreter_evaluate_scope (func_node->d.func_data.scope,
                                           new_variable_scope_stack,
                                           true,
                                           &should_return);
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
                                 &obj,
                                 return_from_scope))
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
interpreter_evaluate_binary_comparison (ASTHandle binary_comparison_handle,
                               DICT (ObjVars) **variable_scope_stack)
{
  struct AST *binary_comparison_ast = ast_get_node (binary_comparison_handle);
    /* get amp objects to work with */
  AmpObject *left_obj =
    InterpreterGetOrGenerateAmpObject
      (binary_comparison_ast->d.bcmp_data.left,
       variable_scope_stack);
  AmpObject *right_obj =
    InterpreterGetOrGenerateAmpObject
      (binary_comparison_ast->d.bcmp_data.right,
       variable_scope_stack);
  AmpObject *retval = NULL;


  if (left_obj->info->type == right_obj->info->type)
    {
      if (binary_comparison_ast->d.bcmp_data.type == BOP_EQUAL)
        retval = left_obj->info->ops.equal (left_obj, right_obj);
      else if (binary_comparison_ast->d.bcmp_data.type == BOP_NOT_EQUAL)
        retval = left_obj->info->ops.not_equal (left_obj, right_obj);
      else if (binary_comparison_ast->d.bcmp_data.type == BOP_LESS_THAN)
        retval = left_obj->info->ops.less_than (left_obj, right_obj);
      else if (binary_comparison_ast->d.bcmp_data.type == BOP_GREATER_THAN)
        retval = left_obj->info->ops.greater_than (left_obj, right_obj);
    }
  else
    {
      printf (ample_error_codes[ERROR_OPERATION_ON_DIFFERENT_TYPES],
              AMP_OBJECT_TYPE_STR[left_obj->info->type],
              AMP_OBJECT_TYPE_STR[right_obj->info->type]);
      exit (1);
    }
  AmpObjectDecrementRefcount (left_obj);
  AmpObjectDecrementRefcount (right_obj);
  return retval;
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
      if (obj->info->type == AMP_OBJECT_BOOL)
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
  else if (expr->type == AST_BINARY_COMPARATOR)
    {
      AmpObject *obj = interpreter_evaluate_binary_comparison (statement_handle,
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
                            bool32 local_scope_already_created,
                            bool32 *should_return)
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
      AmpObject *obj = NULL;
      obj = interpreter_evaluate_statement (scope->d.scope_data.statements[i],
                                            new_variable_scope_stack,
                                            should_return);
      if (obj)
        {
          if (!(*should_return))
            AmpObjectDecrementRefcount (obj);
        }
      if (*should_return) 
        {
          /* if there is a ret value, it's ref count will be high enough
           * so that it's not freed by the free local variables func */
          interpreter_free_local_variables (new_variable_scope_stack[0]);
          ARRAY_FREE (new_variable_scope_stack);
          return obj;
        }
    }

#ifdef INTERPRETER_DEBUG
  debug__interpreter_print_all_vars (new_variable_scope_stack[0]);
#endif
  interpreter_free_local_variables (new_variable_scope_stack[0]);
  ARRAY_FREE (new_variable_scope_stack);

  return NULL;
}

AmpObject *
interpreter_evaluate_if (ASTHandle statement,
                         DICT (ObjVars) **variable_scope_stack,
                         bool32 *return_from_scope)
{
  struct AST *if_node = ast_get_node (statement);
  if (if_node->type == AST_IF)
    {
      /* run different scopes depending on the if's true or false */
      struct AST *expr_node = ast_get_node (statement);
      AmpObject *is_expr_true;
      AmpObject *scope_ret = NULL;

      /* expr_node should evaluate to a bool32 */
      is_expr_true = 
        interpreter_evaluate_statement_to_bool32 (expr_node->d.if_data.expr,
                                                  variable_scope_stack);

      if (AMP_BOOL (is_expr_true)->val)
      scope_ret = interpreter_evaluate_scope (expr_node->d.if_data.scope_if_true,
                                    variable_scope_stack,
                                    false,
                                    return_from_scope);
      else if (AMP_BOOL(is_expr_true)->val == false &&
               expr_node->d.if_data.scope_if_false)
      scope_ret = interpreter_evaluate_scope (expr_node->d.if_data.scope_if_false,
                                    variable_scope_stack,
                                    false,
                                    return_from_scope);


      AmpObjectDecrementRefcount (is_expr_true);
      if (scope_ret)
        return scope_ret;
    }
  return NULL;
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
    case AST_BOOL:
      obj = AmpBoolCreate (node->d.bool_data.value);
      break;
    case AST_BINARY_OP:
      obj = interpreter_evaluate_binary_op (handle, variable_scope_stack);
      break;
    case AST_FUNC_CALL:
      obj = interpreter_evaluate_function_call (handle, variable_scope_stack, NULL);
      break;
    case AST_BINARY_COMPARATOR:
      obj = interpreter_evaluate_binary_comparison (handle, variable_scope_stack);
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
  const char *var = s->d.asgn_data.var;
  size_t scope_stack_index = 0;

  /* make sure that the variable doesn't already exist in a parent scope */
  bool32 exists_in_parent_scope = false;
  for (i = 1; i < ARRAY_COUNT (variable_scope_stack); i++)
    {
      AmpObject *parent_obj = NULL;
      exists_in_parent_scope = DictObjVars_get (variable_scope_stack[i],
                                                var,
                                                &parent_obj);
      if (exists_in_parent_scope)
        {
          scope_stack_index = i;
          break;
        }
    }
  AmpObject *obj = 
    interpreter_evaluate_statement (s->d.asgn_data.expr, variable_scope_stack, NULL);
  if (expr->type == AST_IDENTIFIER)
    interpreter_duplicate_variable (expr->d.id_data.id,
                                    var,
                                    variable_scope_stack,
                                    scope_stack_index);
  else 
    interpreter_add_obj_mapping (var,
                                 obj,
                                 variable_scope_stack[scope_stack_index]);
                         
}

