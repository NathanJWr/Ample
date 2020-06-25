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
#include "array.h"
#include "dict_vars.h"
#include "objects/ampobject.h"
#include "objects/intobject.h"
#include "objects/boolobject.h"
#include "objects/strobject.h"

#include <assert.h>
static DICT(ObjVars) global_variables;

void
interpreter_erase_variable_if_exists (const char *var, DICT (ObjVars) *local_variables)
{

  AmpObject *obj = NULL;
  bool32 success = DictObjVars_get_and_erase (local_variables, var, &obj);
  if (success)
    AmpObjectDecrementRefcount (obj);
}

void
interpreter_add_obj_mapping (const char *var_name, AmpObject *obj, DICT (ObjVars) *local_variables)
{
  interpreter_erase_variable_if_exists (var_name, local_variables);
  DictObjVars_insert (local_variables, var_name, obj);
}

void
InterpreterStart (ASTHandle head)
{
  /* evaluate the global scope */
  interpreter_evaluate_scope (head, NULL);
}

void
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
      interpreter_evaluate_binary_op (statement);
    }
  else if (s->type == AST_IF)
    {
      interpreter_evaluate_if (statement, variable_scope_stack);
    }
  else if (s->type == AST_EQUALITY)
    {
      AmpObject *ret_bool =
        interpreter_evaluate_equality (statement, variable_scope_stack);
      AmpObjectDecrementRefcount (ret_bool);
    }
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
        interpreter_get_or_generate_amp_object
          (equality_ast->d.equality_data.left);
      AmpObject *right_obj =
        interpreter_get_or_generate_amp_object
          (equality_ast->d.equality_data.right);
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
      AmpObject *obj = interpreter_get_amp_object (identifier_str);
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
      AmpObject *obj = interpreter_evaluate_equality (statement_handle, variable_scope_stack);
      return obj;
    }
  else
    {
      printf ("Expression does not evaluate to a bool\n");
      exit (1);
    }
}

void
interpreter_evaluate_scope (ASTHandle scope_handle, DICT (ObjVars) **variable_scope_stack)
{
  struct AST *scope = ast_get_node (scope_handle);
  if (scope->type == AST_SCOPE)
    {
      /* set up the scope's variable stacks */
      DICT(ObjVars) local_variables;
      DICT(ObjVars) **new_variable_scope_stack = NULL;
      size_t i;
      DictObjVars_init (&local_variables, hash_string, string_compare, 10);
      /* the local variables will be at index 0 */
      ARRAY_PUSH (new_variable_scope_stack, &local_variables);
      if (variable_scope_stack)
        {
          for (i = 0; i < ARRAY_COUNT (variable_scope_stack); i++)
            {
              ARRAY_PUSH (new_variable_scope_stack, variable_scope_stack[i]); 
            }
        }

      for (i = 0; i < ARRAY_COUNT (scope->d.scope_data.statements); i++)
        {
          interpreter_evaluate_statement (scope->d.scope_data.statements[i], new_variable_scope_stack);
        }

      debug__interpreter_print_all_vars (new_variable_scope_stack[0]);
      /* local variables have reached the end of their scope */
      for (i = 0; i < new_variable_scope_stack[0]->capacity; i++)
        {
          if (new_variable_scope_stack[0]->map[i] != 0)
            {
              AmpObject *val
                  = DictObjVars_get_entry_pointer (new_variable_scope_stack[0], new_variable_scope_stack[0]->map[i])->val;
              AmpObjectDecrementRefcount (val);
            }
        }
      DictObjVars_free (new_variable_scope_stack[0]);
      ARRAY_FREE (new_variable_scope_stack);
    }
  else
    {
      printf ("Expression is not a scope\n");
      exit (1);
    }
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
        interpreter_evaluate_scope (expr_node->d.if_data.scope_if_true, variable_scope_stack);

      AmpObjectDecrementRefcount (is_expr_true);
    }
}

AmpObject *
interpreter_get_amp_object (const char *var)
{
  AmpObject *obj = NULL;
  bool32 success = DictObjVars_get (&global_variables, var, &obj);
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
interpreter_get_or_generate_amp_object (ASTHandle handle)
{
  struct AST *node = ast_get_node (handle);
  AmpObject *obj = NULL;
  switch (node->type)
    {
    case AST_IDENTIFIER:
      obj = interpreter_get_amp_object (node->d.id_data.id);
      AmpObjectIncrementRefcount (obj);
      break;
    case AST_INTEGER:
      obj = AmpIntegerCreate (node->d.int_data.value);
      break;
    case AST_STRING:
      obj = AmpStringCreate (node->d.str_data.str);
      break;
    case AST_BINARY_OP:
      obj = interpreter_evaluate_binary_op (handle);
      break;
    default:
      assert (false);
      break;
    }
  return obj;
}

AmpObject *
interpreter_evaluate_binary_op (ASTHandle handle)
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
          right = interpreter_get_or_generate_amp_object (right_handle);
        }
      else if (right_node->type == AST_BINARY_OP)
        {
          right = interpreter_evaluate_binary_op (right_handle);
        }
      else
        {
          exit (1);
        }

      if (left_node->type != AST_BINARY_OP)
        {
          left = interpreter_get_or_generate_amp_object (left_handle);
        }
      else if (left_node->type == AST_BINARY_OP)
        {
          left = interpreter_evaluate_binary_op (left_handle);
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
interpreter_duplicate_variable (const char *var, const char *assign, DICT (ObjVars) *local_variables)
{
  AmpObject *obj = NULL;
  bool32 local_found = false;
  bool32 global_found = false;
  local_found = DictObjVars_get (local_variables, var, &obj);
  if (!local_found)
    global_found = DictObjVars_get (&global_variables, var, &obj);
  if (!local_found && !global_found)
    {
      printf ("Variable %s does not exist\n", var);
      exit (1);
    }
  /* new variable will be referencing the same memory */
  AmpObjectIncrementRefcount (obj);
  DictObjVars_insert (&global_variables, assign, obj);
}

void
interpreter_evaluate_assignment (ASTHandle statement, DICT (ObjVars) **variable_scope_stack)
{
  struct AST *s = ast_get_node (statement);
  struct AST *expr = ast_get_node (s->d.asgn_data.expr);
  if (expr->type == AST_INTEGER)
    {
      int val = expr->d.int_data.value;
      AmpObject *obj = AmpIntegerCreate (val);
      interpreter_add_obj_mapping (s->d.asgn_data.var, obj, variable_scope_stack[0]);
    }
  else if (expr->type == AST_BINARY_OP)
    {
      AmpObject *obj = interpreter_evaluate_binary_op (s->d.asgn_data.expr);
      interpreter_add_obj_mapping (s->d.asgn_data.var, obj, variable_scope_stack[0]);
    }
  else if (expr->type == AST_STRING)
    {
      const char *val = expr->d.str_data.str;
      AmpObject *obj = AmpStringCreate (val);
      interpreter_add_obj_mapping (s->d.asgn_data.var, obj, variable_scope_stack[0]);
    }
  else if (expr->type == AST_BOOL)
    {
      bool32 val = expr->d.bool_data.value;
      AmpObject *obj = AmpBoolCreate (val);
      interpreter_add_obj_mapping (s->d.asgn_data.var, obj, variable_scope_stack[0]);
    }
  else if (expr->type == AST_IDENTIFIER)
    {
      const char *var = s->d.asgn_data.var;
      const char *expr_var = expr->d.id_data.id;
      /* fix this! */
      interpreter_duplicate_variable (expr_var, var, variable_scope_stack[0]);
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
              printf ("Int Variable: %s\n\tValue: %d\n", e->key, AMP_INTEGER (obj)->val);
              break;
            case AMP_OBJ_STR:
              printf ("Str Variable: %s\n\tValue: %s\n", e->key, AMP_STRING (obj)->string);
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
