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
static DICT(ObjVars) varmap;

void
interpreter__erase_variable_if_exists (const char *var)
{

  AmpObject *obj = NULL;
  bool success = DictObjVars_get_and_erase (&varmap, var, &obj);
  if (success)
    obj_dec_refcount (obj);
}

void
interpreter__add_obj_mapping (const char *var_name, AmpObject *obj)
{
  interpreter__erase_variable_if_exists (var_name);
  DictObjVars_insert (&varmap, var_name, obj);
}

void
interpreter_start (ASTHandle head)
{
  struct AST *h = NULL;
  unsigned int i = 0;
  /* initialize all variable maps */
  DictObjVars_init (&varmap, hash_string, string_compare, 1);
  h = ast_get_node (head);
  if (h->type == AST_SCOPE)
    {
      for (i = 0; i < ARRAY_COUNT (h->d.scope_data.statements); i++)
        {
          interpreter__evaluate_statement (h->d.scope_data.statements[i]);
        }
    }

  /* end of the program */
  debug__interpreter_print_all_vars ();
  /* all variables have reached the end of their scope */
  for (i = 0; i < varmap.capacity; i++)
    {
      if (varmap.map[i] != 0)
        {
          AmpObject *val
              = DictObjVars_get_entry_pointer (&varmap, varmap.map[i])->val;
          obj_dec_refcount (val);
        }
    }
  DictObjVars_free (&varmap);
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
  AmpObject *obj = NULL;
  bool success = DictObjVars_get (&varmap, var, &obj);
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
interpreter__get_or_generate_amp_object (ASTHandle handle)
{
  struct AST *node = ast_get_node (handle);
  AmpObject *obj = NULL;
  switch (node->type)
    {
    case AST_IDENTIFIER:
      obj = interpreter__get_amp_object (node->d.id_data.id);
      obj_inc_refcount (obj);
      break;
    case AST_INTEGER:
      obj = amp_object_create_integer (node->d.int_data.value);
      break;
    case AST_STRING:
      obj = amp_object_create_string (node->d.str_data.str);
      break;
    default:
      assert (false);
      break;
    }
  return obj;
}

AmpObject *
interpreter__evaluate_binary_op (ASTHandle handle)
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

      if (left_node->type != AST_BINARY_OP)
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

      if (left->info->type == right->info->type)
        {
          AmpObject *obj = NULL;

          switch (node->d.bop_data.op)
            {
              case '+': obj = right->info->add (right, left);
                        break;
              case '-': obj = right->info->sub (right, left);
                        break;
              case '*': obj = right->info->mult (right, left);
                        break;
              case '/': obj = right->info->div (right, left);
                        break;
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
  AmpObject *obj = NULL;
  bool success = DictObjVars_get (&varmap, var, &obj);
  if (!success)
    {
      printf ("Variable %s does not exist\n", var);
      exit (1);
    }
  /* new variable will be referencing the same memory */
  obj_inc_refcount (obj);
  DictObjVars_insert (&varmap, assign, obj);
}

void
interpreter__evaluate_assignment (ASTHandle statement)
{
  struct AST *s = ast_get_node (statement);
  struct AST *expr = ast_get_node (s->d.asgn_data.expr);
  if (expr->type == AST_INTEGER)
    {
      int val = expr->d.int_data.value;
      AmpObject *obj = amp_object_create_integer (val);
      interpreter__add_obj_mapping (s->d.asgn_data.var, obj);
    }
  else if (expr->type == AST_BINARY_OP)
    {
      AmpObject *obj = interpreter__evaluate_binary_op (s->d.asgn_data.expr);
      interpreter__add_obj_mapping (s->d.asgn_data.var, obj);
    }
  else if (expr->type == AST_STRING)
    {
      const char *val = expr->d.str_data.str;
      AmpObject *obj = amp_object_create_string (val);
      interpreter__add_obj_mapping (s->d.asgn_data.var, obj);
    }
  else if (expr->type == AST_IDENTIFIER)
    {
      const char *var = s->d.asgn_data.var;
      const char *expr_var = expr->d.id_data.id;
      interpreter__duplicate_variable (expr_var, var);
    }
}

void
debug__interpreter_print_all_vars ()
{
  unsigned int i;
  for (i = 0; i < varmap.capacity; i++)
    {
      DictEntryHandle h = varmap.map[i];
      if (h != 0)
        {
          const DICT_ENTRY (ObjVars) *e = &varmap.mem[h];
          AmpObject *obj = e->val;
          switch (obj->info->type)
            {
            case AMP_OBJ_INT:
              printf ("Int Variable: %s\n\tValue: %d\n", e->key, AMP_INTEGER (obj)->val);
              break;
            case AMP_OBJ_STR:
              printf ("Str Variable: %s\n\tValue: %s\n", e->key, AMP_STRING (obj)->string);
              break;
            }
        }
    }
}
