#include "dict_vars.h"
#include "hash.h"
DICT_IMPL(IntVars, const char*, IntVariable*);
DICT_IMPL(StrVars, const char*, const char*);
DICT_IMPL(Vars, const char*, enum VarTypes);

struct IntVariable* variable_int_create (int val)
{
  struct IntVariable* v = malloc (sizeof(struct IntVariable));
  *v = (struct IntVariable) {
    .refcount = 1,
    .val = val,
  };
  return v;
}

