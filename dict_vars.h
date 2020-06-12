#include "array.h"
#include "hash.h"
enum VarTypes
{
  VAR_INTEGER,
  VAR_STRING,
};

typedef struct IntVariable
{
  uint8_t refcount;
  int val;
} IntVariable;

#define increment_refcount(r_ptr) (r_ptr)->refcount++
#define decrement_refcount(r_ptr)                                             \
  (r_ptr)->refcount--;                                                        \
  if ((r_ptr)->refcount == 0)                                                 \
  free (r_ptr)

struct IntVariable *variable_int_create (int val);

DICT_DECLARE (IntVars, const char *, IntVariable *);
DICT_DECLARE (StrVars, const char *, const char *);
/* maps a variable to a particular type */
DICT_DECLARE (Vars, const char *, enum VarTypes);
