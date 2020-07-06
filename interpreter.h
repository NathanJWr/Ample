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
/* Returns an amp object that will be created if none exist already */
AmpObject *InterpreterGetOrGenerateAmpObject(ASTHandle handle, DICT (ObjVars) **variable_scope_stack);

/* ******************
   Internal Functions
   ****************** */
/* top level evaluate function that will call more specific
 * evaluations depending on a node's type */
void interpreter_evaluate_statement(ASTHandle statement, DICT (ObjVars) **variable_scope_stack);
/* evaluates an ast node of type AST_BINARY_OP */
AmpObject *interpreter_evaluate_binary_op(ASTHandle handle, DICT (ObjVars) **variable_scope_stack);
/* evaluates an ast node of type  AST_ASSIGNMENT */
void interpreter_evaluate_assignment(ASTHandle statement, DICT (ObjVars) **variable_scope_stack);
/* evaluates an ast node of type AST_IF */
void interpreter_evaluate_if(ASTHandle statement, DICT (ObjVars) **variable_scope_stack);
/* removes a dict entry and decrements the refcount of the amp object
 * that it's associated with */
void interpreter_erase_variable_if_exists(const char *var, DICT (ObjVars) *local_variables);
/* creates a new dict entry pointing to an existing AmpObject and
 * incrementing that objects refcount */
void interpreter_duplicate_variable(const char *var, const char *assign, DICT (ObjVars) **variable_scope_stack, size_t scope_stack_index);
/* Returns an amp object that already exists as a variable */
AmpObject *interpreter_get_amp_object(const char *var, DICT (ObjVars) **variable_scope_stack);
/* Add an object to the variable map for easy storage/access */
void interpreter_add_obj_mapping(const char *var_name, AmpObject *obj, DICT (ObjVars) *local_variables);
/* Increments through a scope ast node's list of statements 
 * and evaulates them */
void interpreter_evaluate_scope (ASTHandle scope_handle, DICT (ObjVars) **variable_scope_stack, bool32 local_scope_already_created);
/* evaluates a statement if the resulting evaluation is a bool32 */
AmpObject *interpreter_evaluate_statement_to_bool (ASTHandle statement_handle, DICT (ObjVars) **variable_scope_stack);
/* evaluates each side of an equality statement and returns true if the statement is true */
AmpObject *interpreter_evaluate_equality (ASTHandle equality_handle, DICT (ObjVars) **variable_scope_stack);
/* insert the func_handle into a dict using the func name as the key */
void interpreter_insert_function_into_dict (ASTHandle func_handle);

void interpreter_evaluate_function_call (ASTHandle func_call, DICT (ObjVars) **variable_scope_stack);

/* will decrement all references of the variables in the dict,
 * then will call Dict Free and then free the pointer "local_variables" */
void interpreter_free_local_variables (DICT (ObjVars) *local_variables);

/* puts local variables at index 0 of a new array of variable dictionaries */
DICT (ObjVars) **interpreter_create_new_variable_scope_stack (DICT (ObjVars) *local_variables, DICT (ObjVars) **var_scope_stack);

/* ***************
 * Debug Functions
 * *************** */
void debug__interpreter_print_all_vars(DICT (ObjVars) *vars);
AmpObject *debug__interpreter_get_variable_object (const char *var);
#endif
