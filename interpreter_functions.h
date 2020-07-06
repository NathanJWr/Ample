#ifndef INTERPRETER_FUNCTIONS_H_
#define INTERPRETER_FUNCTIONS_H_
#include "ast.h"
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
#endif

