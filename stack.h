#ifndef STACK_H_
#define STACK_H_
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define STACK(name) \
    struct Stack##name

#define STACK_DECLARATION(name, type) \
    STACK(name)  { \
        size_t capacity; \
        size_t size; \
        int tail; \
        type* mem; /* array that holds the type specified */ \
    }

#define STACK_STRUCT_INIT(name, type, initial_capacity) \
    (STACK(name)) { \
        .capacity = initial_capacity, \
        .size = 0, \
        .tail = -1, \
        .mem = malloc (initial_capacity * sizeof(type)), \
    }

#define STACK_FREE(stack_p, name) \
    free((stack_p)->mem); \
    memset ((stack_p), 0, sizeof(STACK(name)))

#define STACK_RESIZE(stack_p, new_capacity) do { \
    void* mem = realloc ((stack_p)->mem, new_capacity * sizeof(*(stack_p)->mem)); \
    assert (mem); \
    (stack_p)->capacity = new_capacity; \
    (stack_p)->mem = mem; \
} while(0)

#define STACK_PUSH(stack_p, entry) \
    if ((stack_p)->size >= (stack_p)->capacity) { \
        STACK_RESIZE (stack_p, (stack_p)->capacity*2); \
    } \
    (stack_p)->tail++; \
    (stack_p)->mem[(stack_p)->tail] = entry; \
    (stack_p)->size++

#define STACK_POP(stack_p) \
    assert ((stack_p)->size != 0); \
    (stack_p)->tail--;

#define STACK_FRONT(stack_p) \
    (stack_p)->mem[(stack_p)->tail]


#endif // STACK_H_