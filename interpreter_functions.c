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
#include "ast.h"
#include "ncl.h"
#include "bool.h"
#include "dict_vars.h"
#include "interpreter.h"
#include "ample_errors.h"
#include "objects/ampobject.h"
#include "objects/numobject.h"
#include "objects/boolobject.h"
#include "objects/strobject.h"
#include "objects/listobject.h"

#include "interpreter_functions.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
bool32 ExecuteAmpleFunction (ASTHandle  *restrict args,
                             size_t arg_count,
                             const char *restrict func_name,
                             DICT (ObjVars) **restrict variable_scope_stack,
                             AmpObject **ret_object,
                             bool32 *return_from_scope)
{
  *ret_object = NULL;
  if (0 == strncmp ("print", func_name, 5))
    {
      ample_print (args, arg_count, func_name, variable_scope_stack);
      if (return_from_scope)
        *return_from_scope = false;
      return true;
    }
  else if (0 == strncmp ("str", func_name, 3))
    {
      *ret_object = ample_cast_object_to_string (args,
                                                 arg_count,
                                                 func_name,
                                                 variable_scope_stack);
      if (return_from_scope)
        *return_from_scope = false;
      return true;
    }
  else if (0 == strncmp ("int", func_name, 3))
    {
      *ret_object = ample_cast_object_to_integer (args,
                                                  arg_count,
                                                  func_name,
                                                  variable_scope_stack);
      if (return_from_scope)
        *return_from_scope = false;
      return true;
    }
  else if (0 == strncmp ("bool", func_name, 4))
    {
      *ret_object = ample_cast_object_to_bool (args,
                                               arg_count,
                                               func_name,
                                               variable_scope_stack);
      if (return_from_scope)
        *return_from_scope = false;
      return true;
    }
  else if (0 == strncmp ("return", func_name, 5))
    {
      if (return_from_scope)
        *return_from_scope = true;
      else
        return false;

      if (arg_count == 0)
        {
          return true;
        }
      else
        {
          ample_function_check_arg_numbers (arg_count, 1, func_name);
          *ret_object = InterpreterGetOrGenerateAmpObject (args[0],
                                                           variable_scope_stack);
          return true;
        }
    }
  return false;
}

void
ample_function_check_arg_numbers (size_t count,
                                  size_t expected_count,
                                  const char *func_name)
{
  if (count != expected_count)
    {
      printf (ample_error_codes[ERROR_INVALID_NUMBER_OF_ARGUMENTS],
              func_name,
              expected_count,
              count);
      exit (1);
    }
}

void ample_print_object (AmpObject *obj)
{
  switch (obj->info->type)
    {
    case AMP_OBJECT_NUMBER:
      printf ("%f", AMP_NUMBER (obj)->val);
      break;
    case AMP_OBJECT_STRING:
      printf ("%s", AMP_STRING (obj)->string);
      break;
    case AMP_OBJECT_BOOL:
      printf ("%s", AMP_BOOL (obj)->val ? "true" : "false");
      break;
    case AMP_OBJECT_LIST:
      {
        size_t i;
        printf("[ ");
        for (i = 0; i < ARRAY_COUNT (AMP_LIST (obj)->array) - 1; i++)
          {
            ample_print_object (AMP_LIST (obj)->array[i]);
            printf (", ");
          }
        ample_print_object (AMP_LIST (obj)->array[i]);
        printf (" ]");
      }
      break;
    default:
      printf (ample_error_codes[ERROR_INVALID_OBJECT_TYPE],
              AMP_OBJECT_TYPE_STR[AMP_OBJECT_LIST]);
      exit (EXIT_FAILURE);
    }
}

void
ample_print (ASTHandle  *restrict args,
             size_t arg_count,
             const char *restrict func_name,
             DICT (ObjVars) **restrict variable_scope_stack)
{
  AmpObject *obj;
  ample_function_check_arg_numbers (arg_count, 1, func_name);
  /* get the argument */
  obj = InterpreterGetOrGenerateAmpObject (args[0],
                                           variable_scope_stack);
  ample_print_object (obj);
  printf ("\n");
  AmpObjectDecrementRefcount (obj); 
}

AmpObject *
ample_cast_object_to_string (ASTHandle *restrict args,
                             size_t arg_count,
                             const char *restrict func_name,
                             DICT (ObjVars) **restrict variable_scope_stack)
{
  AmpObject *obj, *ret_object = NULL;
  ample_function_check_arg_numbers (arg_count, 1, func_name);
  /* get the argument */
  obj = InterpreterGetOrGenerateAmpObject (args[0],
                                           variable_scope_stack);
  switch (obj->info->type)
    {
    case AMP_OBJECT_NUMBER: {
      char* str = NCL_DoubleToString (AMP_NUMBER (obj)->val);
      ret_object = AmpStringCreate (str);
      free (str);
    } break;
    case AMP_OBJECT_STRING:
      AmpObjectIncrementRefcount (obj);
      ret_object = obj;
      break;
    case AMP_OBJECT_BOOL:
      ret_object = AmpStringCreate (AMP_BOOL (obj)->val ? "true" : "false");
      break;
    default:
      printf (ample_error_codes[ERROR_INVALID_CAST],
              AMP_OBJECT_TYPE_STR[obj->info->type],
              AMP_OBJECT_TYPE_STR[AMP_OBJECT_STRING]);
      exit (1);
    }
  AmpObjectDecrementRefcount (obj);
  return ret_object;
}

AmpObject *
ample_cast_object_to_integer (ASTHandle *restrict args,
                              size_t arg_count,
                              const char *restrict func_name,
                              DICT (ObjVars) **restrict variable_scope_stack)
{
  AmpObject *obj, *ret_object = NULL;
  ample_function_check_arg_numbers (arg_count, 1, func_name);
  /* get the argument */
  obj = InterpreterGetOrGenerateAmpObject (args[0],
                                           variable_scope_stack);
  switch (obj->info->type)
    {
    case AMP_OBJECT_NUMBER: {
      int num = (int) AMP_NUMBER (obj)->val; 
      ret_object = AmpNumberCreate (num);
    } break;
    case AMP_OBJECT_STRING: {
      long val;
      const char *str = AMP_STRING(obj)->string;
      char *endptr;
      errno = 0; /* To distinguish success/failure after call */
      val = strtol(str, &endptr, 10);

      /* Check for various possible errors */

      if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
          || (errno != 0 && val == 0))
        {
          printf (ample_error_codes[ERROR_INVALID_CAST],
                  AMP_OBJECT_TYPE_STR[obj->info->type],
                  AMP_OBJECT_TYPE_STR[AMP_OBJECT_STRING]);
          exit(EXIT_FAILURE);
        }
      if (endptr == str)
        {
          printf (ample_error_codes[ERROR_INVALID_CAST],
                  AMP_OBJECT_TYPE_STR[obj->info->type],
                  AMP_OBJECT_TYPE_STR[AMP_OBJECT_STRING]);
          exit(EXIT_FAILURE);
        }

      ret_object = AmpNumberCreate ((int)(val));
    } break;
    case AMP_OBJECT_BOOL: {
      int num = AMP_BOOL (obj)->val;
      /* clamp the number to 0 or 1 */
      if (num > 0)
        num = 1;
      ret_object = AmpNumberCreate (num);
    } break;
    default:
      printf (ample_error_codes[ERROR_INVALID_CAST],
              AMP_OBJECT_TYPE_STR[obj->info->type],
              AMP_OBJECT_TYPE_STR[AMP_OBJECT_NUMBER]);
      exit (1);
    }
  AmpObjectDecrementRefcount (obj);
  return ret_object;
}

AmpObject *
ample_cast_object_to_bool (ASTHandle *restrict args,
                           size_t arg_count,
                           const char *restrict func_name,
                           DICT (ObjVars) **restrict variable_scope_stack)
{
  AmpObject *obj, *ret_object = NULL;
  ample_function_check_arg_numbers (arg_count, 1, func_name);
  /* get the argument */
  obj = InterpreterGetOrGenerateAmpObject (args[0],
                                           variable_scope_stack);
  switch (obj->info->type)
    {
    case AMP_OBJECT_BOOL: {
      ret_object = AmpBoolCreate (AMP_BOOL (obj)->val);
    } break;
    case AMP_OBJECT_NUMBER: {
      bool32 val = (bool32) AMP_NUMBER (obj)->val;
      if (val > 0)
        val = 1;
      ret_object = AmpBoolCreate (val);
    } break;
    default:
      printf (ample_error_codes[ERROR_INVALID_CAST],
              AMP_OBJECT_TYPE_STR[obj->info->type],
              AMP_OBJECT_TYPE_STR[AMP_OBJECT_NUMBER]);
      exit (EXIT_FAILURE);
    }
  AmpObjectDecrementRefcount (obj);
  return ret_object;
}
