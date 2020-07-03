#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include "array.h"
#include "mem_debug_public.h"

#ifdef MEM_DEBUG
static size_t total_allocated;
static size_t currently_allocated;
static size_t unique_id_counter;

typedef struct MemInfo {
  size_t unique_id;
  size_t allocation_size;
  char *file;
  int line;
  char mem[1];
} MemInfo;

static MemInfo *allocations[999999];
void*
debug_malloc (size_t size, const char *file, int line)
{
  size_t off = offsetof (MemInfo, mem);
  MemInfo *info = malloc (off + size);
  void *retval = NULL;
  info->unique_id = unique_id_counter;
  info->allocation_size = size;
  info->file = malloc (strlen (file) + 1);
  strcpy (info->file, file);
  info->line = line;
  
  total_allocated += info->allocation_size;
  currently_allocated += info->allocation_size;

  allocations[unique_id_counter++] = info;

  retval = info->mem;
  return retval;
}

void
debug_free (void *ptr)
{
  MemInfo *info = (MemInfo *) (((char*) ptr) - offsetof (MemInfo, mem));
  currently_allocated -= info->allocation_size;
  allocations[info->unique_id] = NULL;
  free (info->file);
  free (info);
}

void *
debug_realloc (void *ptr, size_t size)
{
  MemInfo *info = (MemInfo *) ((char*) ptr - offsetof (MemInfo, mem));
  MemInfo *new_ptr = NULL;
  
  /* remove old size and add new size */
  total_allocated -= info->allocation_size;
  total_allocated += size;
  currently_allocated -= info->allocation_size;
  currently_allocated += size;
  
  new_ptr = realloc (info, offsetof (MemInfo, mem) + size);
  /*
  free (new_ptr->file);
  new_ptr->file = malloc (strlen (file) + 1);
  strcpy (new_ptr->file, file);
  */
  new_ptr->allocation_size = size;
  allocations[new_ptr->unique_id] = new_ptr;
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
MemDebugPrintInfo ()
{
  printf ("Total allocated memory: %u bytes\n", (unsigned int) total_allocated);
  printf ("Currently allocated memory: %u bytes\n", (unsigned int) currently_allocated);
}

void
MemDebugPrintLeaks ()
{
  size_t i;
  for (i = 0; i < unique_id_counter; i++)
    {
      if (allocations[i])
        {
          printf ("MEMORY LEAK: %s: %d\n", allocations[i]->file, allocations[i]->line);
        }
    }
}
#endif
