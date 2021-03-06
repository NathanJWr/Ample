/*
    This file is part of Ample.

    Ample is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ample is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ample.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef QUEUE_H_
#define QUEUE_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define QUEUE_DECLARATION(name, type)                                          \
  struct Queue##name {                                                         \
    size_t capacity;                                                           \
    size_t size;                                                               \
    int head;                                                                  \
    int tail;                                                                  \
    type *mem; /* array that holds the type specified */                       \
  }

#define QUEUE(name) struct Queue##name

#define QUEUE__DEFAULT_CAPACITY 1
#define QUEUE_STRUCT_INIT(name, q_ptr, type, initial_capacity)                 \
  (q_ptr)->capacity = initial_capacity;                                        \
  (q_ptr)->size = 0;                                                           \
  (q_ptr)->head = 0;                                                           \
  (q_ptr)->tail = -1;                                                          \
  (q_ptr)->mem = malloc(initial_capacity * sizeof(type))

#define QUEUE_FREE(queue_p, name)                                              \
  free((queue_p)->mem);                                                        \
  memset((queue_p), 0, sizeof(QUEUE(name)))

#define QUEUE_RESIZE(queue_p, new_capacity)                                    \
  do {                                                                         \
    void *mem = NULL;                                                          \
    /* this is probably super expensive, but it keeps the start of the array   \
     * memory usable */                                                        \
    if ((queue_p)->head > 0) {                                                 \
      size_t queue_index;                                                      \
      /* move the array so it starts at index 0 */                             \
      size_t size = (queue_p)->size;                                           \
      /* using a for loop here because you can't memcpy regions that may       \
       * overlap */                                                            \
      for (queue_index = 0; queue_index < size; queue_index++) {               \
        (queue_p)->mem[queue_index] =                                          \
          (queue_p)->mem[(queue_p)->head + queue_index];                       \
      }                                                                        \
      (queue_p)->head = 0;                                                     \
      (queue_p)->tail = (int) size - 1;                                        \
    }                                                                          \
    mem = realloc((queue_p)->mem, new_capacity * sizeof(*(queue_p)->mem));     \
    assert(mem);                                                               \
    (queue_p)->capacity = new_capacity;                                        \
    (queue_p)->mem = mem;                                                      \
  } while (0)

#define QUEUE_PUSH(queue_p, entry)                                             \
  if ((queue_p)->tail + 1 >= (int) (queue_p)->capacity) {                      \
    QUEUE_RESIZE(queue_p, (queue_p)->capacity * 2);                            \
  }                                                                            \
  (queue_p)->tail++;                                                           \
  (queue_p)->mem[(queue_p)->tail] = entry;                                     \
  (queue_p)->size++

#define QUEUE_POP(queue_p)                                                     \
  assert((queue_p)->size != 0);                                                \
  (queue_p)->head++;                                                           \
  (queue_p)->size--;                                                           \
  if ((queue_p)->head > (queue_p)->tail) {                                     \
    (queue_p)->head = 0;                                                       \
    (queue_p)->tail = 0;                                                       \
  }

#define QUEUE_FRONT(queue_p) (queue_p)->mem[(queue_p)->head]

#define QUEUE_EMPTY(queue_p) ((queue_p)->size == 0)

#endif
