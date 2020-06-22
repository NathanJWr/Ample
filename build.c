#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#define MEM_DEBUG
#include "mem_debug.c"
#include "mem_debug.h"


#include "ampobject.c"
#include "ast.c"
#include "dict_vars.c"
#include "hash.c"
#include "interpreter.c"
#include "intobject.c"
#include "boolobject.c"
#include "lexer.c"
#include "ncl.c"
#include "parser.c"
#include "ssl.c"
#include "strobject.c"


#include "main.c"
