#include "ast.h"
#include "bool.h"
#include "dict_vars.h"
#include "interpreter.h"
#include "objects/ampobject.h"
#include "objects/numobject.h"
#include "objects/boolobject.h"
#include "objects/strobject.h"

#include <stdlib.h>
#include <stddef.h>
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
  obj = interpreter_get_or_generate_amp_object (args[0],
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
