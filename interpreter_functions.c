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
#include "objects/ampobject.h"
#include "objects/numobject.h"
#include "objects/boolobject.h"
#include "objects/strobject.h"

#include "interpreter_functions.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
bool32 ExecuteAmpleFunction (ASTHandle  *__restrict__ args,
                             size_t arg_count,
                             const char *__restrict__ func_name,
                             DICT (ObjVars) **__restrict__ variable_scope_stack,
                             AmpObject **ret_object)
{
  *ret_object = NULL;
  if (0 == strncmp ("print", func_name, 5))
    {
      ample_print (args, arg_count, func_name, variable_scope_stack);
      return true;
    }
  else if (0 == strncmp ("str", func_name, 3))
    {
      *ret_object = ample_cast_object_to_string (args,
                                                 arg_count,
                                                 func_name,
                                                 variable_scope_stack);
      return true;
    }
  else if (0 == strncmp ("int", func_name, 3))
    {
      *ret_object = ample_cast_object_to_integer (args,
                                                  arg_count,
                                                  func_name,
                                                  variable_scope_stack);
      return true;
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

void
ample_print (ASTHandle  *__restrict__ args,
             size_t arg_count,
             const char *__restrict__ func_name,
             DICT (ObjVars) **__restrict__ variable_scope_stack)
{
  AmpObject *obj;
  ample_function_check_arg_numbers (arg_count, 1, func_name);
  /* get the argument */
  obj = InterpreterGetOrGenerateAmpObject (args[0],
                                           variable_scope_stack);
  switch (obj->info->type)
    {
    case AMP_OBJECT_NUMBER:
      printf ("%f\n", AMP_NUMBER (obj)->val);
      break;
    case AMP_OBJECT_STRING:
      printf ("%s\n", AMP_STRING (obj)->string);
      break;
    case AMP_OBJECT_BOOL:
      printf ("%s", AMP_BOOL (obj)->val ? "true" : "false");
      break;
    }

  AmpObjectDecrementRefcount (obj); 
}

AmpObject *
ample_cast_object_to_string (ASTHandle *__restrict__ args,
                             size_t arg_count,
                             const char *__restrict__ func_name,
                             DICT (ObjVars) **__restrict__ variable_scope_stack)
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
ample_cast_object_to_integer (ASTHandle *__restrict__ args,
                              size_t arg_count,
                              const char *__restrict__ func_name,
                              DICT (ObjVars) **__restrict__ variable_scope_stack)
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
    default:
      printf (ample_error_codes[ERROR_INVALID_CAST],
              AMP_OBJECT_TYPE_STR[obj->info->type],
              AMP_OBJECT_TYPE_STR[AMP_OBJECT_NUMBER]);
      exit (1);
    }
  AmpObjectDecrementRefcount (obj);
  return ret_object;
}

