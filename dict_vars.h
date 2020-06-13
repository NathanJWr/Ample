#include "array.h"
#include "hash.h"
enum VarTypes
{
  VAR_INTEGER,
  VAR_STRING,
};

typedef enum AmpObjectType {
  AMP_OBJ_INT,
  AMP_OBJ_STR,
} AmpObjectType;

#define AMP_OBJECT_HEADER \
  uint8_t refcount; \
  AmpObjectType type; \
  void (*dealloc) (); \
  void* value

typedef struct AmpObject
{
  AMP_OBJECT_HEADER;
} AmpObject;

typedef struct AmpObject_Int
{
  AMP_OBJECT_HEADER;
} AmpObject_Int;

typedef struct AmpObject_Str
{
  AmpObject amp_obj;
  char *str;
} AmpObject_Str;
void obj_inc_refcount (AmpObject* obj);
void obj_dec_refcount (AmpObject* obj);

AmpObject *amp_object_create_integer (int val);
AmpObject *amp_object_create_string (const char *str);

DICT_DECLARE (IntVars, const char *, AmpObject *);
DICT_DECLARE (StrVars, const char *, AmpObject *);
/* maps a variable to a particular type */
DICT_DECLARE (Vars, const char *, enum VarTypes);
