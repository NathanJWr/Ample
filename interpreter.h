#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "ast.h"
/* ******************
   External functions
   ****************** */
void interpreter_start (ASTHandle head);

/* ******************
   Internal Functions
   ****************** */
void interpreter__evaluate_statement (ASTHandle statement);
int interpreter__evaluate_binary_op (ASTHandle handle);
void interpreter__evaluate_assignment (statement);
#endif // INTERPRETER_H_