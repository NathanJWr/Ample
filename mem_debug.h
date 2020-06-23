#ifdef MEM_DEBUG
#define MEM_DEBUG
void *debug_malloc(size_t size, const char *file, int line);
void debug_free(void *ptr);
void *debug_realloc(void *ptr, size_t size);
void *debug_calloc (size_t nmemb, size_t size, const char *file, int line);
void mem_debug_print_info();

#define malloc(size) debug_malloc (size, __FILE__, __LINE__)
#define free(ptr) debug_free (ptr)
#define realloc(ptr, size) debug_realloc (ptr, size)
#define calloc(nmemb, size) debug_calloc (nmemb, size, __FILE__, __LINE__)
#endif
