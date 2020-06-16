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
#ifndef STACK_H_
#define STACK_H_
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define STACK(name) struct Stack##name

#define STACK_DECLARATION(name, type)                                          \
  STACK(name) {                                                                \
    size_t capacity;                                                           \
    size_t size;                                                               \
    int tail;                                                                  \
    type *mem; /* array that holds the type specified */                       \
  }

#define STACK_STRUCT_INIT(name, stack_ptr, type, initial_capacity)             \
  (stack_ptr)->capacity = initial_capacity;                                    \
  (stack_ptr)->size = 0;                                                       \
  (stack_ptr)->tail = -1;                                                      \
  (stack_ptr)->mem = malloc(initial_capacity * sizeof(type));

#define STACK_FREE(stack_p, name)                                              \
  free((stack_p)->mem);                                                        \
  memset((stack_p), 0, sizeof(STACK(name)))

#define STACK_RESIZE(stack_p, new_capacity)                                    \
  do {                                                                         \
    void *mem =                                                                \
        realloc((stack_p)->mem, new_capacity * sizeof(*(stack_p)->mem));       \
    assert(mem);                                                               \
    (stack_p)->capacity = new_capacity;                                        \
    (stack_p)->mem = mem;                                                      \
  } while (0)

#define STACK_PUSH(stack_p, entry)                                             \
  if ((stack_p)->size >= (stack_p)->capacity) {                                \
    STACK_RESIZE(stack_p, (stack_p)->capacity * 2);                            \
  }                                                                            \
  (stack_p)->tail++;                                                           \
  (stack_p)->mem[(stack_p)->tail] = entry;                                     \
  (stack_p)->size++

#define STACK_POP(stack_p)                                                     \
  assert((stack_p)->size != 0);                                                \
  (stack_p)->tail--;                                                           \
  (stack_p)->size--

#define STACK_FRONT(stack_p) (stack_p)->mem[(stack_p)->tail]

#define STACK_EMPTY(stack_p) ((stack_p)->size == 0)

#endif
