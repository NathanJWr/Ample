#include "intobject.h"
#include <stdlib.h>
AmpObject *
amp_object_add_integer (AmpObject_Int *this, AmpObject_Int *val)
{
  int add = *(int*)this->value + *(int*)val->value;
  AmpObject* obj = amp_object_create_integer (add);
  return obj;
}

AmpObject *
amp_object_create_integer (int val)
{
  struct AmpObject_Int *a = malloc (sizeof (AmpObject_Int));
  a->type = AMP_OBJ_INT;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->value = malloc (sizeof (int));
  *(int *)(a->value) = val;

  a->addition = amp_object_add_integer;
  return a;
}