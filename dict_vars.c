#include "dict_vars.h"
#include "hash.h"
#include "ssl.h"
DICT_IMPL (IntVars, const char *, AmpObject *);
DICT_IMPL (StrVars, const char *, AmpObject *);
DICT_IMPL (Vars, const char *, enum VarTypes);

void
amp_object_destroy_basic (AmpObject *obj)
{
  free (obj->value);
  free (obj);
}
AmpObject *
amp_object_create_integer (int val)
{
  struct AmpObject *a = malloc (sizeof (AmpObject));
  a->type = AMP_OBJ_INT;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->value = malloc (sizeof (int));
  *(int *)(a->value) = val;

  return a;
}

AmpObject *
amp_object_create_string (const char *str)
{
  AmpObject *a = malloc (sizeof (AmpObject));
  a->type = AMP_OBJ_STR;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->value = strdup (str);

  return a;
}
void
obj_inc_refcount (AmpObject *obj)
{
  obj->refcount++;
}
void
obj_dec_refcount (AmpObject *obj)
{
  obj->refcount--;
  if (obj->refcount == 0)
    {
      obj->dealloc (obj);
    }
}
