#ifndef STR_OBJECT_H_
#define STR_OBJECT_H_
#include "ampobject.h"
typedef struct AmpObject_Str
{
  AMP_OBJECT_HEADER;
	AmpObject *(*concat) (AmpObject *, AmpObject *);
} AmpObject_Str;
#define AMP_STRING(obj) ((AmpObject_Str *)(obj))
AmpObject *amp_object_create_string (const char* str);
AmpObject *amp_object_create_string_nodup (char *str);
#endif // STR_OBJECT_H_