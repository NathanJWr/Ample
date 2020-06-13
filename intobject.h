#include "ampobject.h"

typedef struct AmpObject_Int
{
  AMP_OBJECT_HEADER;
  struct AmpObject_Int* (*addition)(struct AmpObject_Int*, struct AmpObject_Int*);
} AmpObject_Int;

AmpObject *amp_object_create_integer (int val);
