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
bool32 ExecuteAmpleFunction (ASTHandle  *restrict args,
                             size_t arg_count,
                             const char *restrict func_name,
                             DICT (ObjVars) **restrict variable_scope_stack,
                             AmpObject **ret_object,
                             bool32 *return_from_scope);
void
ample_function_check_arg_numbers (size_t count,
                                  size_t expected_count,
                                  const char *func_name);
void ample_print (ASTHandle  *restrict args,
                  size_t arg_count,
                  const char *restrict func_name,
                  DICT (ObjVars) **restrict variable_scope_stack);
AmpObject *
ample_cast_object_to_string (ASTHandle *restrict args,
                             size_t arg_count,
                             const char *restrict func_name,
                             DICT (ObjVars) **restrict variable_scope_stack);
/* cast to create an integer. 
 * This will throw away any decimal and just return the integer part of
 * a number */
AmpObject *
ample_cast_object_to_integer (ASTHandle *restrict args,
                              size_t arg_count,
                              const char *restrict func_name,
                              DICT (ObjVars) **restrict variable_scope_stack);
AmpObject *
ample_cast_object_to_bool (ASTHandle *restrict args,
                           size_t arg_count,
                           const char *restrict func_name,
                           DICT (ObjVars) **restrict variable_scope_stack);
#endif

