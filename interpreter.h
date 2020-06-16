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
void interpreter__evaluate_statement(ASTHandle statement);
AmpObject *interpreter__evaluate_binary_op(ASTHandle handle);
void interpreter__evaluate_assignment(ASTHandle statement);
void interpreter__erase_variable_if_exists(const char *var);
void interpreter__duplicate_variable(const char *var, const char *assign);

/* Returns an amp object that already exists as a variable */
AmpObject *interpreter__get_amp_object(const char *var);
/* Returns an amp object that will be created if none exist already */
AmpObject *interpreter__get_or_generate_amp_object(ASTHandle handle);
/* Assumes left and right are AMP_OBJ_INT and will attempt to conduct an
 * operation based on op variable. Returns a newly created amp object with the
 * result of the operation */
AmpObject *interpreter__integer_operation(TValue op, AmpObject *right,
                                          AmpObject *left);
/* Assumes left and right are AMP_OBJ_STR and will attempt to conduct an
 * operation based on op variable. Returns a newly created amp object with the
 * result of the operation */
AmpObject *interpreter__string_operation(TValue op, AmpObject *right,
                                         AmpObject *left);
/* Add an object to the variable map for easy storage/access */
void interpreter__add_obj_mapping(const char *var_name, AmpObject *obj);

/* ***************
 * Debug Functions
 * *************** */
void debug__interpreter_print_all_vars();
#endif

