#ifndef MEM_DEBUG_H_
#define MEM_DEBUG_H_
void *debug_malloc(size_t size, const char *file, int line);
void debug_free(void *ptr, const char *file, int line);
void *debug_realloc(void *ptr, size_t size, const char *file, int line);
void *debug_calloc (size_t nmemb, size_t size, const char *file, int line);
void mem_debug_print_info();

#define malloc(size) debug_malloc (size, __FILE__, __LINE__)
#define free(ptr) debug_free (ptr, __FILE__, __LINE__)
#define realloc(ptr, size) debug_realloc (ptr, size, __FILE__, __LINE__)
#define calloc(nmemb, size) debug_calloc (nmemb, size, __FILE__, __LINE__)
#endif
