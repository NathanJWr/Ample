#include "dict_vars.h"
#include "hash.h"
DICT_IMPL(IntVars, const char*, int);
DICT_IMPL(StrVars, const char*, const char*);
DICT_IMPL(Vars, const char*, enum VarTypes);
