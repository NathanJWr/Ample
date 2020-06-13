#include "ampobject.h"

typedef struct AmpObject_Int
{
  AMP_OBJECT_HEADER;
  AmpObject *(*addition) (AmpObject *, AmpObject *);
  AmpObject *(*subtraction) (AmpObject *, AmpObject *);
  AmpObject *(*division) (AmpObject *, AmpObject *);
  AmpObject *(*multiplication) (AmpObject *, AmpObject *);
} AmpObject_Int;
#define AMP_INTEGER(obj) ((AmpObject_Int *)(obj))
AmpObject *amp_object_create_integer (int val);
