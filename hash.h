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
/* USAGE:
   This file has two user facing macros: DICT_DECLARE and DICT_IMPL
   DICT_DECLARE creates the header definitions and structures necessary
   for any dict functions
   DICT_IMPL creates the definition of each function */
#define HASH_MULTIPLIER (37)
#define DICT_MAX_LOAD_FACTOR (1)
#define DICT_GROWTH_FACTOR (2)

typedef size_t DictEntryHandle;

#define DICT(name) struct Dict##name
#define DICT_ENTRY(name) struct DictEntry##name
#define DICT_FUNCTION2(name, func_name) name__##func_name
#define DICT_FUNCTION(name, func_name) Dict##name##_##func_name
#define DICT_DECLARE(name, key_type, val_type)                                 \
  DICT_ENTRY(name) {                                                           \
    key_type key;                                                              \
    val_type val;                                                              \
    DictEntryHandle next;                                                      \
  };                                                                           \
  DICT(name) {                                                                 \
    size_t capacity;                                                         \
    size_t count;                                                            \
    size_t (*hash_function)(key_type key);                                   \
    bool32 (*key_compare)(key_type key, key_type input);                         \
    DICT_ENTRY(name) * mem; /* flat array of all entries */                    \
    DictEntryHandle *map;   /* actual map structure */                         \
  };                                                                           \
  void DICT_FUNCTION(name, init)(                                              \
      DICT(name) * dict, size_t(*hash_function)(key_type key),               \
      bool32 (*key_compare)(key_type key, key_type input),                       \
      size_t initial_capacity);                                                \
  void DICT_FUNCTION(name, free)(DICT(name) * dict);                           \
  DictEntryHandle DICT_FUNCTION(name, get_entry_handle)(DICT(name) * dict);    \
  DICT_ENTRY(name)                                                             \
  *DICT_FUNCTION(name, get_entry_pointer)(DICT(name) * dict,                   \
                                          DictEntryHandle handle);             \
  void DICT_FUNCTION(name, grow)(DICT(name) * dict);                           \
  void DICT_FUNCTION(name, insert)(DICT(name) * dict, key_type key,            \
                                   val_type val);                              \
  bool32 DICT_FUNCTION(name, get)(DICT(name) * dict, key_type key,               \
                                val_type * val);                               \
  bool32 DICT_FUNCTION(name, erase)(DICT(name) * dict, key_type key);            \
  bool32 DICT_FUNCTION(name, get_and_erase)(DICT(name) * dict, key_type key,     \
                                          val_type * val)

#define DICT_IMPL(name, key_type, val_type)                                    \
    void DICT_FUNCTION(name, init)(                                            \
        DICT(name) * dict, size_t(*hash_function)(key_type key),             \
        bool32 (*key_compare)(key_type key, key_type input),                     \
        size_t initial_capacity) {                                             \
      (dict)->capacity = initial_capacity;                                     \
      (dict)->count = 0;                                                       \
      (dict)->hash_function = hash_function;                                   \
      (dict)->key_compare = key_compare;                                       \
      (dict)->mem = NULL;                                                      \
      (dict)->map = calloc(1, sizeof(*(dict)->map) * initial_capacity);        \
    }                                                                          \
    void DICT_FUNCTION(name, free)(DICT(name) * dict) {                        \
      if (dict->map)                                                           \
        free(dict->map);                                                       \
      if (dict->mem)                                                           \
        ARRAY_FREE(dict->mem);                                                 \
    }                                                                          \
    DictEntryHandle DICT_FUNCTION(name, get_entry_handle)(DICT(name) * dict) { \
      DICT_ENTRY(name) e = {0};                                                \
      ARRAY_PUSH(dict->mem, e);                                                \
      if (ARRAY_COUNT(dict->mem) == 1) {                                       \
        ARRAY_PUSH(dict->mem, e);                                              \
      }                                                                        \
      return ARRAY_COUNT(dict->mem) - 1;                                       \
    }                                                                          \
    DICT_ENTRY(name)                                                           \
    *DICT_FUNCTION(name, get_entry_pointer)(DICT(name) * dict,                 \
                                            DictEntryHandle handle) {          \
      return &(dict->mem[handle]);                                             \
    }                                                                          \
    void DICT_FUNCTION(name, grow)(DICT(name) * dict);                         \
    void DICT_FUNCTION(name, insert)(DICT(name) * dict, key_type key,          \
                                     val_type val) {                           \
      size_t hash = dict->hash_function(key) % dict->capacity;               \
      DictEntryHandle handle = DICT_FUNCTION(name, get_entry_handle)(dict);    \
      DICT_ENTRY(name) *e =                                                    \
          DICT_FUNCTION(name, get_entry_pointer)(dict, handle);                \
                                                                               \
      e->key = key;                                                            \
      e->val = val;                                                            \
      e->next = dict->map[hash];                                               \
                                                                               \
      dict->map[hash] = handle;                                                \
      dict->count++;                                                           \
      if (dict->count >= dict->capacity * DICT_MAX_LOAD_FACTOR) {              \
        DICT_FUNCTION(name, grow)(dict);                                       \
      }                                                                        \
    }                                                                          \
    void DICT_FUNCTION(name, grow)(DICT(name) * dict) {                        \
      /* create a new dict that will have a greater capcaity */                \
      DICT(name) new_dict = {0};                                               \
      size_t new_capacity = dict->capacity * DICT_GROWTH_FACTOR;             \
      size_t i;                                                              \
      DICT_FUNCTION(name, init)                                                \
      (&new_dict, dict->hash_function, dict->key_compare, new_capacity);       \
      /* Need to rehash all entries because the capacity changed */            \
      for (i = 0; i < dict->count; i++) {                                      \
        DictEntryHandle e_handle;                                              \
        for (e_handle = dict->map[i]; e_handle != 0;) {                        \
          DICT_ENTRY(name) *e =                                                \
              DICT_FUNCTION(name, get_entry_pointer)(dict, e_handle);          \
          DICT_FUNCTION(name, insert)                                          \
          (&new_dict, e->key, e->val);                                         \
          e_handle = e->next;                                                  \
        }                                                                      \
      }                                                                        \
      new_dict.count = dict->count;                                            \
      DICT_FUNCTION(name, free)(dict);                                         \
      *dict = new_dict;                                                        \
    }                                                                          \
    bool32 DICT_FUNCTION(name, get)(DICT(name) * dict, key_type key,             \
                                  val_type * val) {                            \
      size_t hash = dict->hash_function(key) % dict->capacity;               \
      DictEntryHandle handle = dict->map[hash];                                \
      while (handle != 0) {                                                    \
        DICT_ENTRY(name) *e =                                                  \
            DICT_FUNCTION(name, get_entry_pointer)(dict, handle);              \
        if (dict->key_compare(e->key, key)) {                                  \
          *val = e->val;                                                       \
          return true;                                                         \
        }                                                                      \
        handle = e->next;                                                      \
      }                                                                        \
      return false;                                                            \
    }                                                                          \
    bool32 DICT_FUNCTION(name, erase)(DICT(name) * dict, key_type key) {         \
      size_t hash = dict->hash_function(key) % dict->capacity;               \
      DictEntryHandle handle = dict->map[hash];                                \
      DICT_ENTRY(name) *e = NULL;                                              \
      DICT_ENTRY(name) *prev = NULL;                                           \
      while (handle != 0) {                                                    \
        e = DICT_FUNCTION(name, get_entry_pointer)(dict, handle);              \
        if (dict->key_compare(e->key, key)) {                                  \
          /* remove this key, it's a match */                                  \
          dict->count--;                                                       \
          if (prev == NULL && e->next != 0) {                                  \
            /* there is at least 1 entry in the linked list */                 \
            dict->map[hash] = e->next;                                         \
          } else if (prev == NULL && e->next == 0) {                           \
            /* there are no entries in the linked list */                      \
            dict->map[hash] = 0;                                               \
          } else if (prev) {                                                   \
            /* the entry that needs to be deleted is in the linked list        \
             */                                                                \
            prev->next = 0;                                                    \
          }                                                                    \
          return true;                                                         \
        }                                                                      \
        prev = e;                                                              \
        handle = e->next;                                                      \
      }                                                                        \
      return false;                                                            \
    }                                                                          \
    bool32 DICT_FUNCTION(name, get_and_erase)(DICT(name) * dict, key_type key,   \
                                            val_type * val) {                  \
      size_t hash = dict->hash_function(key) % dict->capacity;               \
      DictEntryHandle handle = dict->map[hash];                                \
      DICT_ENTRY(name) *e = NULL;                                              \
      DICT_ENTRY(name) *prev = NULL;                                           \
      while (handle != 0) {                                                    \
        e = DICT_FUNCTION(name, get_entry_pointer)(dict, handle);              \
        *val = e->val;                                                         \
        if (dict->key_compare(e->key, key)) {                                  \
          /* remove this key, it's a match */                                  \
          dict->count--;                                                       \
          if (prev == NULL && e->next != 0) {                                  \
            /* there is at least 1 entry in the linked list */                 \
            dict->map[hash] = e->next;                                         \
          } else if (prev == NULL && e->next == 0) {                           \
            /* there are no entries in the linked list */                      \
            dict->map[hash] = 0;                                               \
          } else if (prev) {                                                   \
            /* the entry that needs to be deleted is in the linked list        \
             */                                                                \
            prev->next = 0;                                                    \
          }                                                                    \
          return true;                                                         \
        }                                                                      \
        prev = e;                                                              \
        handle = e->next;                                                      \
      }                                                                        \
      return false;                                                            \
    }                                                                          \

size_t hash_string(const char *s);
bool32 string_compare(const char *key, const char *input);
bool32 int_compare(int key, int input);
void hash_insert_string_key(const char *key, int value);
#endif
