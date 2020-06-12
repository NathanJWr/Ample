#include "hash.h"
enum VarTypes {
  VAR_INTEGER,
  VAR_STRING,
};
DICT_DECLARE (IntVars, const char*, int);
DICT_DECLARE (StrVars, const char*, const char*);
/* maps a variable to a particular type */
DICT_DECLARE (Vars, const char*, enum VarTypes);
