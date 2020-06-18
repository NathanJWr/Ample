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
#include "ampobject.h"
#include "ast.h"
/* ******************
   External functions
   ****************** */
void interpreter_start(ASTHandle head);

/* ******************
   Internal Functions
   ****************** */
/* top level evaluate function that will call more specific
 * evaluations depending on a node's type */
void interpreter__evaluate_statement(ASTHandle statement);
/* evaluates an ast node of type AST_BINARY_OP */
AmpObject *interpreter__evaluate_binary_op(ASTHandle handle);
/* evaluates an ast node of type  AST_ASSIGNMENT */
void interpreter__evaluate_assignment(ASTHandle statement);
/* evaluates an ast node of type AST_IF */
void interpreter__evaluate_if(ASTHandle statement);
/* removes a dict entry and decrements the refcount of the amp object
 * that it's associated with */
void interpreter__erase_variable_if_exists(const char *var);
/* creates a new dict entry pointing to an existing AmpObject and
 * incrementing that objects refcount */
void interpreter__duplicate_variable(const char *var, const char *assign);
/* Returns an amp object that already exists as a variable */
AmpObject *interpreter__get_amp_object(const char *var);
/* Returns an amp object that will be created if none exist already */
AmpObject *interpreter__get_or_generate_amp_object(ASTHandle handle);
/* Add an object to the variable map for easy storage/access */
void interpreter__add_obj_mapping(const char *var_name, AmpObject *obj);

/* ***************
 * Debug Functions
 * *************** */
void debug__interpreter_print_all_vars();
#endif
