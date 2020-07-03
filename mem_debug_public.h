#ifndef MEM_DEBUG_PUBLIC_
#define MEM_DEBUG_PUBLIC_
#include <stddef.h>
void* debug_malloc (size_t size, const char *file, int line);
void debug_free (void *ptr);
void *debug_realloc (void *ptr, size_t size);
void *debug_calloc (size_t nmemb, size_t size, const char *file, int line);

#ifdef MEM_DEBUG
void mem_debug_print_info(); 
void mem_debug_print_leaks();

#define MemDebugPrintInfo() mem_debug_print_info()
#define MemDebugPrintLeaks() mem_debug_print_leaks()

#else
#define MemDebugPrintInfo()
#define MemDebugPrintLeaks()

#endif


#endif
