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
#include <stdbool.h>
#define HASH_MULTIPLIER (37)
#define DICT_MAX_LOAD_FACTOR (1)
#define DICT_GROWTH_FACTOR (2)

typedef uint32_t DictEntryHandle;

#define DICT(name) struct Dict##name
#define DICT_ENTRY(name) struct DictEntry##name
#define DICT_DECLARATION(name, key_type, val_type)                             \
  DICT_ENTRY(name) {                                                           \
    key_type key;                                                              \
    val_type val;                                                              \
    DictEntryHandle next;                                                      \
  };                                                                           \
  DICT(name) {                                                                 \
    uint32_t capacity;                                                         \
    uint32_t count;                                                            \
    uint64_t (*hash_function)(key_type key);                                   \
    bool (*key_compare)(key_type key, key_type input);                         \
    DICT_ENTRY(name) * mem; /* flat array of all entries */                    \
    DictEntryHandle *map;   /* actual map structure */                         \
  }

#define DICT_SIZE(dict_ptr) (dict_ptr)->capacity
#define DICT_COUNT(dict_ptr) (dict_ptr)->count

#define DICT_FREE(dict_ptr)                                                    \
  free((dict_ptr)->map);                                                       \
  ARRAY_FREE((dict_ptr)->mem)

/* Allocates and returns the handle to a DictEntry */
#define DICT_GET_ENTRY_HANDLE(name, dict_ptr)                                  \
  ({                                                                           \
    DictEntryHandle retval;                                                    \
    DICT_ENTRY(name) e = {0};                                                  \
    ARRAY_PUSH((dict_ptr)->mem, e);                                            \
    if (ARRAY_COUNT((dict_ptr)->mem) == 1) {                                   \
      ARRAY_PUSH((dict_ptr)->mem, e);                                          \
    }                                                                          \
    retval = ARRAY_COUNT((dict_ptr)->mem) - 1;                                 \
    retval;                                                                    \
  })

/* Gets the pointer to a DictEntry from a DictEntryHandle */
#define DICT_GET_ENTRY_POINTER(dict_ptr, handle)                               \
  ((typeof((dict_ptr)->mem))(&(dict_ptr)->mem[handle]))

#define DICT_INIT(dict_ptr, hash, compare, initial_capacity)                   \
  (dict_ptr)->hash_function = hash;                                            \
  (dict_ptr)->key_compare = compare;                                           \
  (dict_ptr)->capacity = initial_capacity;                                     \
  (dict_ptr)->mem = NULL;                                                      \
  (dict_ptr)->map = calloc(1, sizeof(*(dict_ptr)->map) * initial_capacity)

#define DICT_GROW(name, dict_ptr)                                              \
  do {                                                                         \
    /* create a new dict that will have a greater capacity */                  \
    DICT(name) new_dict = {0};                                                 \
    uint32_t new_capacity = (dict_ptr)->capacity * DICT_GROWTH_FACTOR;         \
    DICT_INIT(&new_dict, (dict_ptr)->hash_function, (dict_ptr)->key_compare,   \
              new_capacity);                                                   \
    new_dict.count = (dict_ptr)->count;                                        \
    new_dict.mem = (dict_ptr)->mem;                                            \
    /* Need to rehash all entries because the capacity changed */              \
    for (uint32_t i = 0; i < (dict_ptr)->count; i++) {                         \
      DictEntryHandle *collided_handles = NULL;                                \
      DictEntryHandle *collided_handles_next = NULL;                           \
      for (DictEntryHandle e_handle = (dict_ptr)->map[i]; e_handle != 0;) {    \
        DICT_ENTRY(name) *e_new = DICT_GET_ENTRY_POINTER(dict_ptr, e_handle);  \
        uint64_t hash =                                                        \
            new_dict.hash_function(e_new->key) % new_dict.capacity;            \
        if (new_dict.map[hash] != 0) {                                         \
          /* collision */                                                      \
          /* need to track these because the pointers to the next handle need  \
             to be preserved in this loop, but changed later */                \
          ARRAY_PUSH(collided_handles, e_handle);                              \
          ARRAY_PUSH(collided_handles_next, new_dict.map[hash]);               \
        }                                                                      \
        new_dict.map[hash] = e_handle;                                         \
        e_handle = e_new->next;                                                \
      }                                                                        \
      /* if there were collisions, fix the next handles */                     \
      if (collided_handles) {                                                  \
        for (int i = 0; i < ARRAY_COUNT(collided_handles); i++) {              \
          DICT_ENTRY(name) *e =                                                \
              DICT_GET_ENTRY_POINTER(dict_ptr, collided_handles[i]);           \
          /* set the next handle of the collided entry to the one logged from  \
           * before */                                                         \
          e->next = collided_handles_next[i];                                  \
          /* set the logged handle's next to 0 */                              \
          e = DICT_GET_ENTRY_POINTER(dict_ptr, collided_handles_next[i]);      \
          e->next = 0;                                                         \
        }                                                                      \
        ARRAY_FREE(collided_handles);                                          \
        ARRAY_FREE(collided_handles_next);                                     \
      }                                                                        \
    }                                                                          \
    free((dict_ptr)->map);                                                     \
    *(dict_ptr) = new_dict;                                                    \
  } while (0)

#define DICT_INSERT(name, dict_ptr, k, v)                                      \
  do {                                                                         \
    uint64_t hash = (dict_ptr)->hash_function(k) % (dict_ptr)->capacity;       \
    DictEntryHandle handle = DICT_GET_ENTRY_HANDLE(name, dict_ptr);            \
    DICT_ENTRY(name) *e = DICT_GET_ENTRY_POINTER(dict_ptr, handle);            \
    *e = (DICT_ENTRY(name)){                                                   \
        .key = k,                                                              \
        .val = v,                                                              \
        .next = (dict_ptr)->map[hash],                                         \
    };                                                                         \
    (dict_ptr)->map[hash] = handle;                                            \
    (dict_ptr)->count++;                                                       \
    if ((dict_ptr)->count >= (dict_ptr)->capacity * DICT_MAX_LOAD_FACTOR) {    \
      DICT_GROW(name, dict_ptr);                                               \
    }                                                                          \
  } while (0)

#define DICT_GET(name, dict_ptr, k)                                            \
  ({                                                                           \
    uint64_t hash = (dict_ptr)->hash_function(k) % (dict_ptr)->capacity;       \
    DictEntryHandle handle = (dict_ptr)->map[hash];                            \
    typeof((dict_ptr)->mem[0].val) retval;                                     \
    while (handle != 0) {                                                      \
      DICT_ENTRY(name) *e = DICT_GET_ENTRY_POINTER(dict_ptr, handle);          \
      if (strcmp(e->key, k) == 0)                                              \
        retval = e->val;                                                       \
      handle = e->next;                                                        \
    }                                                                          \
    retval;                                                                    \
  })

#define DICT_ERASE(name, dict_ptr, k)                                          \
  do {                                                                         \
    uint64_t hash = (dict_ptr)->hash_function(k) % (dict_ptr)->capacity;       \
    DictEntryHandle handle = (dict_ptr)->map[hash];                            \
    DICT_ENTRY(name) * e;                                                      \
    DICT_ENTRY(name) *prev = NULL;                                             \
    bool removed = false;                                                      \
    while (handle != 0) {                                                      \
      e = DICT_GET_ENTRY_POINTER(dict_ptr, handle);                            \
      if ((dict_ptr)->key_compare(e->key, k)) {                                \
        /* remove this key, it's a match */                                    \
        removed = true;                                                        \
        if (prev == NULL && e->next != 0) {                                    \
          /* there is at least 1 entry in the linked list */                   \
          (dict_ptr)->map[hash] = e->next;                                     \
        } else if (prev == NULL && e->next == 0) {                             \
          /* there are no entries in the linked list */                        \
          (dict_ptr)->map[hash] = 0;                                           \
        } else if (prev) {                                                     \
          /* the entry that needs to be deleted is in the linked list */       \
          prev->next = 0;                                                      \
        }                                                                      \
      }                                                                        \
      prev = e;                                                                \
      handle = e->next;                                                        \
    }                                                                          \
    if (removed)                                                               \
      (dict_ptr)->count--;                                                     \
  } while (0)

#define DICT_AT(name, dict_ptr, k, value_ptr)                                  \
  ({                                                                           \
    bool retval = false;                                                       \
    uint64_t hash = (dict_ptr)->hash_function(k) % (dict_ptr)->capacity;       \
    DictEntryHandle handle = (dict_ptr)->map[hash];                            \
    DICT_ENTRY(name) *e = NULL;                                                \
    while (handle != 0) {                                                      \
      DICT_ENTRY(name) *n = DICT_GET_ENTRY_POINTER(dict_ptr, handle);          \
      if ((dict_ptr)->key_compare(k, n->key)) {                                \
        e = n;                                                                 \
      }                                                                        \
      handle = n->next;                                                        \
    }                                                                          \
    if (e != NULL) {                                                           \
      *(value_ptr) = e->val;                                                   \
      retval = true;                                                           \
    } else {                                                                   \
      retval = false;                                                          \
    }                                                                          \
    retval;                                                                    \
  })

uint64_t hash_string(const char *s);
bool string_compare(const char *key, const char *input);
void hash_insert_string_key(const char *key, int value);
#endif // HASH_H_
