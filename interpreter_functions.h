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
#ifndef INTERPRETER_FUNCTIONS_H_
#define INTERPRETER_FUNCTIONS_H_
#include "ast.h"

typedef enum AmpleErrorCode {
  ERROR_INVALID_NUMBER_OF_ARGUMENTS,
  ERROR_INVALID_CAST,
} AmpleErrorCode;

const char *ample_error_codes[] = {
  "ERROR: Invalid number of arguments for function \"%s\", expected %u \
argument(s) and %u were provided\n",
  "ERROR: Cannot cast object of type %s to %s\n",
};

bool32 ExecuteAmpleFunction (ASTHandle  *__restrict__ args,
                             size_t arg_count,
                             const char *__restrict__ func_name,
                             DICT (ObjVars) **__restrict__ variable_scope_stack,
                             AmpObject **ret_object);
void ample_print (ASTHandle  *__restrict__ args,
                  size_t arg_count,
                  const char *__restrict__ func_name,
                  DICT (ObjVars) **__restrict__ variable_scope_stack);
AmpObject *
ample_cast_object_to_string (ASTHandle *__restrict__ args,
                             size_t arg_count,
                             const char *__restrict__ func_name,
                             DICT (ObjVars) **__restrict__ variable_scope_stack);
/* cast to create an integer. 
 * This will throw away any decimal and just return the integer part of
 * a number */
AmpObject *
ample_cast_object_to_integer (ASTHandle *__restrict__ args,
                              size_t arg_count,
                              const char *__restrict__ func_name,
                              DICT (ObjVars) **__restrict__ variable_scope_stack);
#endif

