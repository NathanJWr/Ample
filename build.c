#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

/* possible debug defines:
 * MEM_DEBUG,
 * PARSER_DEBUG 
 * INTERPRETER_DEBUG */
#define MEM_DEBUG
#include "mem_debug_public.h"
#include "mem_debug.c"
#include "mem_debug.h"


#include "objects/ampobject.c"
#include "objects/numobject.c"
#include "objects/strobject.c"
#include "objects/boolobject.c"
#include "objects/listobject.c"

#include "ast.c"
#include "dict_vars.c"
#include "hash.c"
#include "interpreter.c"
#include "interpreter_functions.c"
#include "lexer.c"
#include "ncl.c"
#include "parser.c"
#include "ssl.c"


#include "main.c"
