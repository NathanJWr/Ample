#include "strobject.h"
#include <string.h>
#include <stdlib.h>
AmpObject *
amp_object_concat_string (AmpObject *this, AmpObject *str)
{
  unsigned int size = strlen ((char*) this->value);
  size += strlen ((char*) str->value);

  char* s = calloc (1, size + 1);
  strcat (s, (char*) this->value);
  strcat (s, (char*) str->value);
  return amp_object_create_string_nodup (s);
}

AmpObject *
amp_object_create_string (const char *str)
{
  AmpObject_Str *a = malloc (sizeof (AmpObject_Str));
  a->type = AMP_OBJ_STR;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->value = strdup (str);

  a->concat = amp_object_concat_string;
  return AMP_OBJECT(a);
}

AmpObject *
amp_object_create_string_nodup (char *str)
{
  AmpObject_Str *a = malloc (sizeof (AmpObject_Str));
  a->type = AMP_OBJ_STR;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->value = str;

  a->concat = amp_object_concat_string;
  return AMP_OBJECT(a);
}