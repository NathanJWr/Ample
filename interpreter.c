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
#include "ampobject.h"
#include "array.h"
#include "dict_vars.h"
#include "intobject.h"
#include "strobject.h"

#include <assert.h>
static DICT (IntVars) int_vars;
static DICT (StrVars) str_vars;
static DICT (Vars) var_types;

void
interpreter__erase_variable_if_exists (const char *var)
{
  /* erase variable type mapping */
  AmpObjectType type;
  if (!DictVars_get_and_erase (&var_types, var, &type))
    {
      /* the variable should not exist in any of the following dicts */
      return;
    }

  /* erase actual variable */
  switch (type)
    {
    case AMP_OBJ_INT:
      {
        AmpObject *int_var;
        bool success = DictIntVars_get_and_erase (&int_vars, var, &int_var);
        if (success)
          {
            obj_dec_refcount (int_var);
            return;
          }
        else
          {
            printf ("Type mapping and value mismatch\n");
            exit (1);
          }
      }
      break;
    case AMP_OBJ_STR:
      {
        AmpObject *str_var;
        bool success = DictStrVars_get_and_erase (&str_vars, var, &str_var);
        if (success)
          {
            obj_dec_refcount (str_var);
            return;
          }
        else
          {
            printf ("Type mapping and value mismatch\n");
            exit (1);
          }
      }
      break;
    }
}
void
interpreter__add_integer_variable (const char *var_name, int val)
{
  interpreter__erase_variable_if_exists (var_name);
  AmpObject *obj = amp_object_create_integer (val);
  DictIntVars_insert (&int_vars, var_name, obj);
  DictVars_insert (&var_types, var_name, AMP_OBJ_INT);
}

void
interpreter__add_integer_obj (const char *var_name, AmpObject *obj)
{
  interpreter__erase_variable_if_exists (var_name);
  DictIntVars_insert (&int_vars, var_name, obj);
  DictVars_insert (&var_types, var_name, AMP_OBJ_INT);
}

void
interpreter__add_string_variable (const char *var_name, const char *val)
{
  interpreter__erase_variable_if_exists (var_name);
  AmpObject *obj = amp_object_create_string (val);
  DictStrVars_insert (&str_vars, var_name, obj);
  DictVars_insert (&var_types, var_name, AMP_OBJ_STR);
}

void
interpreter_start (ASTHandle head)
{
  /* initialize all variable maps */
  DictStrVars_init (&str_vars, hash_string, string_compare, 10);
  DictIntVars_init (&int_vars, hash_string, string_compare, 10);
  DictVars_init (&var_types, hash_string, string_compare, 10);
  struct AST *h = ast_get_node (head);
  if (h->type == AST_SCOPE)
    {
      for (unsigned int i = 0; i < ARRAY_COUNT (h->scope_data.statements); i++)
        {
          interpreter__evaluate_statement (h->scope_data.statements[i]);
        }
    }

  /* end of the program */
  /* all variables have reached the end of their scope */
  for (int i = 0; i < int_vars.capacity; i++)
    {
      if (int_vars.map[i] != 0)
        {
          AmpObject *val
              = DictIntVars_get_entry_pointer (&int_vars, int_vars.map[i])
                    ->val;
          obj_dec_refcount (val);
        }
    }
  for (int i = 0; i < str_vars.capacity; i++)
    {
      if (str_vars.map[i] != 0)
        {
          AmpObject *val
              = DictStrVars_get_entry_pointer (&str_vars, str_vars.map[i])
                    ->val;
          obj_dec_refcount (val);
        }
    }
  DictStrVars_free (&str_vars);
  DictIntVars_free (&int_vars);
  DictVars_free (&var_types);
}

void
interpreter__evaluate_statement (ASTHandle statement)
{
  struct AST *s = ast_get_node (statement);
  if (s->type == AST_ASSIGNMENT)
    {
      interpreter__evaluate_assignment (statement);
    }
  else if (s->type == AST_BINARY_OP)
    {
      interpreter__evaluate_binary_op (statement);
    }
}

AmpObject *
interpreter__get_amp_object (const char *var)
{
  AmpObjectType type;
  bool success = DictVars_get (&var_types, var, &type);
  if (success)
    {
      AmpObject *obj;
      switch (type)
        {
        case AMP_OBJ_INT:
          DictIntVars_get (&int_vars, var, &obj);
          break;
        case AMP_OBJ_STR:
          DictStrVars_get (&str_vars, var, &obj);
          break;
        }
      return obj;
    }
  printf ("Variable \"%s\" does not exist\n", var);
  exit (1);
  return NULL;
}

/* returns an owning pointer to an Amp Object
   i.e the object returned will have it's reference counter incremented */
AmpObject *
interpreter__get_or_generate_amp_object (ASTHandle handle)
{
  struct AST *node = ast_get_node (handle);
  AmpObject *obj = NULL;
  switch (node->type)
    {
    case AST_IDENTIFIER:
      obj = interpreter__get_amp_object (node->id_data.id);
      obj_inc_refcount (obj);
      break;
    case AST_INTEGER:
      obj = amp_object_create_integer (node->int_data.value);
      break;
    case AST_STRING:
      obj = amp_object_create_string (node->str_data.str);
      break;
    default:
      assert (false);
      break;
    }
  return obj;
}

AmpObject*
interpreter__integer_operation (TValue op, AmpObject *right, AmpObject *left)
{
  AmpObject *obj = NULL;
  switch (op)
    {
    case '+':
      obj = AMP_INTEGER (right)->addition (right, left);
      break;
    case '-':
      obj = AMP_INTEGER (right)->subtraction (right, left);
      break;
    case '/':
      obj = AMP_INTEGER (right)->division (right, left);
      break;
    case '*':
      obj = AMP_INTEGER (right)->multiplication (right, left);
      break;
    default:
      printf ("Unsupported integer binary operation, %c\n", op);
      exit (1);
    }
  return obj;
}

AmpObject *
interpreter__string_operation (TValue op, AmpObject *right, AmpObject *left)
{
  AmpObject *obj = NULL;
  switch (op)
    {
    case '+':
      obj = AMP_STRING (right)->concat (right, left);
      break;
    default:
      printf ("Unsupported string binary operation, %c\n", op);
      exit (1);
    }
  return obj;
}
AmpObject *
interpreter__evaluate_binary_op (ASTHandle handle)
{
  struct AST *node = ast_get_node (handle);

  if (node->type == AST_BINARY_OP)
    {
      ASTHandle right_handle = node->bop_data.right;
      ASTHandle left_handle = node->bop_data.left;
      struct AST *right_node = ast_get_node (right_handle);
      struct AST *left_node = ast_get_node (left_handle);
      AmpObject *left = NULL, *right = NULL;

      if (right_node->type != AST_BINARY_OP)
        {
          right = interpreter__get_or_generate_amp_object (right_handle);
        }
      else if (right_node->type == AST_BINARY_OP)
        {
          right = interpreter__evaluate_binary_op (right_handle);
        }
      else
        {
          exit (1);
        }

      if (right_node->type != AST_BINARY_OP)
        {
          left = interpreter__get_or_generate_amp_object (left_handle);
        }
      else if (left_node->type == AST_BINARY_OP)
        {
          left = interpreter__evaluate_binary_op (left_handle);
        }
      else
        {
          exit (1);
        }

      if (left->type == right->type)
        {
          AmpObject *obj = NULL;
          switch (right->type)
            {
            case AMP_OBJ_INT:
              obj = interpreter__integer_operation (node->bop_data.op, right, left);
              break;
            case AMP_OBJ_STR:
              obj = interpreter__string_operation (node->bop_data.op, right, left);
              break;
            default:
              printf ("Type does not support binary operations\n");
              exit (1);
            }
          obj_dec_refcount (left);
          obj_dec_refcount (right);
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
interpreter__duplicate_variable (const char *var, const char *assign)
{
  AmpObjectType type;
  bool success = DictVars_get (&var_types, var, &type);
  if (!success)
    {
      printf ("Variable %s does not exist\n", var);
      exit (1);
    }
  switch (type)
    {
    case AMP_OBJ_INT:
      {
        AmpObject *val;
        success = DictIntVars_get (&int_vars, var, &val);
        if (!success)
          {
            printf ("Variable %s does not exist\n", var);
            exit (1);
          }
        obj_inc_refcount (val);
        DictIntVars_insert (&int_vars, assign, val);
      }
      break;
    case AMP_OBJ_STR:
      {
        AmpObject *val;
        success = DictStrVars_get (&str_vars, var, &val);
        if (!success)
          {
            printf ("Variable %s does not exist\n", var);
            exit (1);
          }
        obj_dec_refcount (val);
        DictStrVars_insert (&str_vars, assign, val);
      }
    }
}

void
interpreter__evaluate_assignment (ASTHandle statement)
{
  struct AST *s = ast_get_node (statement);
  struct AST *expr = ast_get_node (s->asgn_data.expr);
  if (expr->type == AST_INTEGER)
    {
      int val = expr->int_data.value;
      interpreter__add_integer_variable (s->asgn_data.var, val);
    }
  else if (expr->type == AST_BINARY_OP)
    {
      AmpObject *obj = interpreter__evaluate_binary_op (s->asgn_data.expr);
      interpreter__add_integer_obj (s->asgn_data.var, obj);
    }
  else if (expr->type == AST_STRING)
    {
      const char *val = expr->str_data.str;
      interpreter__add_string_variable (s->asgn_data.var, val);
    }
  else if (expr->type == AST_IDENTIFIER)
    {
      const char *var = s->asgn_data.var;
      const char *expr_var = expr->id_data.id;
      interpreter__duplicate_variable (expr_var, var);
    }
}
