#ifndef ARRAY_H_
#define ARRAY_H_
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "ncl.h"
struct Array_Buffer {
    size_t count;
    size_t capacity;
    char buffer[];
};
static inline struct Array_Buffer* ARR_BASE_POINTER(void* arr_ptr) {
    return ((struct Array_Buffer*) ((char*) arr_ptr - sizeof(struct Array_Buffer)));
}

#define ARRAY_ALLOCATION_SIZE(arr_ptr, size) (sizeof(struct Array_Buffer) + sizeof(*arr_ptr)*size)
#define ARR_DEFAULT_INIT_SIZE 10

#define ARRAY_RESIZE(arr_ptr, size) do { \
    struct Array_Buffer* bp = ARR_BASE_POINTER(arr_ptr); \
    bp = ncl_realloc (bp, ARRAY_ALLOCATION_SIZE(arr_ptr, size)); \
    arr_ptr = (typeof(arr_ptr)) bp->buffer; \
} while (0)

#define ARRAY_FREE(arr_ptr) \
    free (ARR_BASE_POINTER(arr_ptr))

#define ARRAY_PUSH(arr_ptr, item) \
    if (arr_ptr == NULL) { \
        struct Array_Buffer* buff = calloc (1, ARRAY_ALLOCATION_SIZE (arr_ptr, ARR_DEFAULT_INIT_SIZE)); \
        buff->count++; \
        buff->capacity = ARR_DEFAULT_INIT_SIZE; \
        arr_ptr = (typeof(arr_ptr)) buff->buffer; \
        arr_ptr[0] = item; \
    } else { \
        struct Array_Buffer* buff = ARR_BASE_POINTER(arr_ptr); \
        if (buff->count >= buff->capacity) { \
            ARRAY_RESIZE (arr_ptr, buff->capacity * 2); \
            buff = ARR_BASE_POINTER ((char*) arr_ptr); \
            buff->capacity *= 2; \
        } \
        arr_ptr[buff->count++] = item; \
    }

#define ARRAY_COUNT(arr_ptr) \
    ARR_BASE_POINTER ((char*) arr_ptr)->count

#define ARRAY_ADD(arr_ptr, count) \
    if (arr_ptr == NULL) { \
        struct Array_Buffer* buff = calloc (1, ARRAY_ALLOCATION_SIZE (arr_ptr, count)); \
        buff->capacity = count; \
        arr_ptr = (typeof(arr_ptr)) buff->buffer; \
    } else { \
        struct Array_Buffer* buff = ARR_BASE_POINTER(arr_ptr); \
        buff->capacity += count; \
        ARRAY_RESIZE (arr_ptr, buff->capacity); \
    }

#endif // ARRAY_H_