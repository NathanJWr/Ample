#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

typedef unsigned int bool32;
enum bool_values { false = 0, true = 1 };
#define MEM_DEBUG
#include "mem_debug.c"
#include "mem_debug.h"


#include "objects/ampobject.c"
#include "objects/intobject.c"
#include "objects/strobject.c"
#include "objects/boolobject.c"

#include "ast.c"
#include "dict_vars.c"
#include "hash.c"
#include "interpreter.c"
#include "lexer.c"
#include "ncl.c"
#include "parser.c"
#include "ssl.c"


#include "main.c"
