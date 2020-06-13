#include "array.h"
#include "hash.h"
enum VarTypes
{
  VAR_INTEGER,
  VAR_STRING,
};
typedef struct AmpObject
{
  uint8_t refcount;
  void (*dealloc) ();
} AmpObject;

typedef struct IntVariable
{
  AmpObject amp_obj;
  int val;
} IntVariable;

typedef struct StrVariable
{
  AmpObject amp_obj;
  char *str;
} StrVariable;
#define obj_inc_refcount(obj_ptr) (obj_ptr)->amp_obj.refcount++
#define obj_dec_refcount(obj_ptr)                                             \
  (obj_ptr)->amp_obj.refcount--;                                              \
  if ((obj_ptr)->amp_obj.refcount == 0)                                       \
  (obj_ptr)->amp_obj.dealloc (obj_ptr)

struct IntVariable *variable_int_create (int val);
struct StrVariable *variable_str_create (const char *str);

DICT_DECLARE (IntVars, const char *, IntVariable *);
DICT_DECLARE (StrVars, const char *, struct StrVariable *);
/* maps a variable to a particular type */
DICT_DECLARE (Vars, const char *, enum VarTypes);
