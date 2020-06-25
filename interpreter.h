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
#include "objects/ampobject.h"
#include "ast.h"
/* ******************
   External functions
   ****************** */
void InterpreterStart(ASTHandle head);

/* ******************
   Internal Functions
   ****************** */
/* top level evaluate function that will call more specific
 * evaluations depending on a node's type */
void interpreter_evaluate_statement(ASTHandle statement, DICT (ObjVars) *local_variables);
/* evaluates an ast node of type AST_BINARY_OP */
AmpObject *interpreter_evaluate_binary_op(ASTHandle handle);
/* evaluates an ast node of type  AST_ASSIGNMENT */
void interpreter_evaluate_assignment(ASTHandle statement, DICT (ObjVars) *local_variables);
/* evaluates an ast node of type AST_IF */
void interpreter_evaluate_if(ASTHandle statement, DICT (ObjVars) *local_variables);
/* removes a dict entry and decrements the refcount of the amp object
 * that it's associated with */
void interpreter_erase_variable_if_exists(const char *var, DICT (ObjVars) *local_variables);
/* creates a new dict entry pointing to an existing AmpObject and
 * incrementing that objects refcount */
void interpreter_duplicate_variable(const char *var, const char *assign, DICT (ObjVars) *local_variables);
/* Returns an amp object that already exists as a variable */
AmpObject *interpreter_get_amp_object(const char *var);
/* Returns an amp object that will be created if none exist already */
AmpObject *interpreter_get_or_generate_amp_object(ASTHandle handle);
/* Add an object to the variable map for easy storage/access */
void interpreter_add_obj_mapping(const char *var_name, AmpObject *obj, DICT (ObjVars) *local_variables);
/* Increments through a scope ast node's list of statements 
 * and evaulates them */
void interpreter_evaluate_scope (ASTHandle scope_handle, bool32 in_global_scope);
/* evaluates a statement if the resulting evaluation is a bool32 */
AmpObject *interpreter_evaluate_statement_to_bool (ASTHandle statement_handle, DICT (ObjVars) *local_variables);
/* evaluates each side of an equality statement and returns true if the statement is true */
AmpObject *interpreter_evaluate_equality (ASTHandle equality_handle, DICT (ObjVars) *local_variables);

/* ***************
 * Debug Functions
 * *************** */
void debug__interpreter_print_all_vars(DICT (ObjVars) *vars);
AmpObject *debug__interpreter_get_variable_object (const char *var);
#endif
