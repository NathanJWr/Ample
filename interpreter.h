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
#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "ast.h"
#include "ampobject.h"
/* ******************
   External functions
   ****************** */
void interpreter_start(ASTHandle head);

/* ******************
   Internal Functions
   ****************** */
void interpreter__evaluate_statement(ASTHandle statement);
AmpObject* interpreter__evaluate_binary_op(ASTHandle handle);
void interpreter__evaluate_assignment(ASTHandle statement);
void interpreter__add_integer_variable(const char *var_name, int val);
void interpreter__erase_variable_if_exists(const char *var);
void interpreter__add_string_variable(const char *var_name, const char *val);
void interpreter__duplicate_variable(const char *var, const char *assign);
#endif // INTERPRETER_H_
