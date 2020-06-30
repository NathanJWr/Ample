#ifndef MEM_DEBUG_PUBLIC_
#define MEM_DEBUG_PUBLIC_
#include <stddef.h>
void* debug_malloc (size_t size, const char *file, int line);
void debug_free (void *ptr);
void *debug_realloc (void *ptr, size_t size);
void *debug_calloc (size_t nmemb, size_t size, const char *file, int line);
void MemDebugPrintInfo ();
void MemDebugPrintLeaks ();
#endif
