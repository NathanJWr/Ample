#include "dict_vars.h"
#include "hash.h"
#include "ssl.h"
DICT_IMPL (IntVars, const char *, IntVariable *);
DICT_IMPL (StrVars, const char *, struct StrVariable *);
DICT_IMPL (Vars, const char *, enum VarTypes);

void
amp_object_integer_dealloc (IntVariable *v)
{
  free (v);
}

void
amp_object_string_dealloc (StrVariable *v)
{
  free (v->str);
  free (v);
}

struct IntVariable *
variable_int_create (int val)
{
  struct IntVariable *v = malloc (sizeof (struct IntVariable));
  v->amp_obj.dealloc = amp_object_integer_dealloc;
  v->amp_obj.refcount = 1;
  v->val = val;

  return v;
}
struct StrVariable *
variable_str_create (const char *str)
{
  struct StrVariable *v = malloc (sizeof (struct StrVariable));
  v->amp_obj.dealloc = amp_object_string_dealloc;
  v->amp_obj.refcount = 1;
  v->str = strdup (str);

  return v;
}
