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
#ifndef HASH_H_
#define HASH_H_
#include "array.h"
#include <inttypes.h>
#define HASH_MULTIPLIER (37)
#define DICT_MAX_LOAD_FACTOR (1)
#define DICT_GROWTH_FACTOR (2)

typedef uint32_t HashNodeHandle;

#define DICT(name) struct Dict##name
#define DICT_ENTRY(name) struct DictEntry##name
#define DICT_DECLARATION(name, key_type, val_type) \
    DICT_ENTRY(name) { \
        key_type key; \
        val_type val; \
        DICT_ENTRY (name)* next; \
    }; \
    DICT(name) { \
        uint32_t capacity; \
        uint32_t count; \
        uint64_t (*hash_function)(key_type key); \
        DICT_ENTRY(name)** mem; \
    }

#define DICT_INIT(dict_ptr, hash, initial_capacity) \
    (dict_ptr)->hash_function = hash; \
    (dict_ptr)->capacity = initial_capacity; \
    (dict_ptr)->mem = calloc (1, initial_capacity * sizeof(*(dict_ptr)->mem))

#define DICT_GROW(name, dict_ptr) do { \
    DICT(name) new_dict = {0}; \
    uint32_t new_capacity = (dict_ptr)->capacity * DICT_GROWTH_FACTOR; \
    DICT_INIT (&new_dict, (dict_ptr)->hash_function, new_capacity); \
    new_dict.count = (dict_ptr)->count; \
    printf("Reached grow for\n"); \
    for (uint32_t i = 0; i < (dict_ptr)->count; i++) { \
        for (DICT_ENTRY (name) *e = (dict_ptr)->mem[i]; e != 0; e = e->next) { \
            uint64_t hash = new_dict.hash_function(e->key) % new_dict.capacity; \
            printf("Grow Hash: %ld\n", hash); \
            DICT_ENTRY (name)* e_new =  malloc (sizeof(DICT_ENTRY(name))); \
            *e_new = (DICT_ENTRY(name)) { \
                .key = e->key, \
                .val = e->val, \
                .next = NULL, \
            }; \
            printf("Allocated and set new entry\n"); \
            e_new->next = new_dict.mem[hash]; \
            new_dict.mem[hash] = e_new; \
        } \
    } \
    *dict_ptr = new_dict; \
} while(0)

#define DICT_INSERT(name, dict_ptr, k, v) do { \
    uint64_t hash = (dict_ptr)->hash_function(k) % (dict_ptr)->capacity; \
    DICT_ENTRY (name)* e =  malloc (sizeof(DICT_ENTRY(name))); \
    *e = (DICT_ENTRY(name)) { \
        .key = k, \
        .val = v, \
        .next = NULL, \
    }; \
    printf("Hash: %ld\n", hash); \
    e->next = (dict_ptr)->mem[hash]; \
    (dict_ptr)->mem[hash] = e; \
    (dict_ptr)->count++; \
    if ((dict_ptr)->count >= (dict_ptr)->capacity * DICT_MAX_LOAD_FACTOR) { \
        DICT_GROW (name, dict_ptr); \
    } \
} while (0)
uint64_t hash_string (const char* s);
void hash_insert_string_key (const char* key, int value);
#endif // HASH_H_