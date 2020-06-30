#ifdef MEM_DEBUG
#define malloc(size) debug_malloc (size, __FILE__, __LINE__)
#define free(ptr) debug_free (ptr)
#define realloc(ptr, size) debug_realloc (ptr, size)
#define calloc(nmemb, size) debug_calloc (nmemb, size, __FILE__, __LINE__)
#endif
