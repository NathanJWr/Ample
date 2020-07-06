#include "ast.h"
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
bool32 ExecuteAmpleFunction (ASTHandle  *__restrict__ args,
                             size_t arg_count,
                             const char *__restrict__ func_name,
                             DICT (ObjVars) **__restrict__ variable_scope_stack)
{
  if (0 == strncmp ("print", func_name, 5))
    {
      ample_print (args, arg_count, func_name, variable_scope_stack);
      return true;
    }
  return false;
}

void ample_print (ASTHandle  *__restrict__ args,
                  size_t arg_count,
                  const char *__restrict__ func_name,
                  DICT (ObjVars) **__restrict__ variable_scope_stack)
{
  AmpObject *obj;
  if (arg_count != 1)
    {
      printf ("Invalid number of arguments for function \"%s\", ",
              func_name);
      printf ("expected %u argument(s) and %u were provided\n",
              1,
              (unsigned int) arg_count);
      exit (1);
    }
  /* get the argument */
  obj = InterpreterGetOrGenerateAmpObject (args[0],
                                           variable_scope_stack);
  switch (obj->info->type)
    {
    case AMP_OBJ_INT:
      printf ("%f\n", AMP_NUMBER (obj)->val);
      break;
    case AMP_OBJ_STR:
      printf ("%s\n", AMP_STRING (obj)->string);
      break;
    case AMP_OBJ_BOOL:
      printf ("%s", AMP_BOOL (obj)->val ? "true" : "false");
      break;
    }

  AmpObjectDecrementRefcount (obj); 
}
