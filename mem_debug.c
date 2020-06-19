#include <stdlib.h>
#include <stdio.h>
static size_t total_allocated;
static size_t currently_allocated;

typedef struct MemInfo {
  size_t allocation_size;
  char mem[1];
} MemInfo;
void*
debug_malloc (size_t size, const char *file, int line)
{
  size_t off = offsetof (MemInfo, mem);
  MemInfo *info = malloc (off + size);
  void *retval = NULL;
  info->allocation_size = size;
  
  total_allocated += info->allocation_size;
  currently_allocated += info->allocation_size;
  retval = info->mem;
  return retval;
}

void
debug_free (void *ptr, const char *file, int line)
{
  MemInfo *info = ((char*) ptr) - offsetof (MemInfo, mem);
  currently_allocated -= info->allocation_size;
  free (info);
}

void *
debug_realloc (void *ptr, size_t size, const char *file, int line)
{
  void *actual_ptr = (char*) ptr - offsetof (MemInfo, mem);
  MemInfo *new_ptr = realloc (actual_ptr, offsetof (MemInfo, mem) + size);
  return new_ptr->mem;
}

void *
debug_calloc (size_t nmemb, size_t size, const char *file, int line)
{
  void *ptr = debug_malloc (nmemb * size, file, line);
  memset (ptr, 0, nmemb * size);
  return ptr;
}

void
mem_debug_print_info ()
{
  printf ("Total allocated memory: %zd bytes\n", total_allocated);
  printf ("Currently allocated memory: %zd bytes\n", currently_allocated);
}