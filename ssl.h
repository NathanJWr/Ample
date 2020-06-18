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
#ifndef SSL_H_
#define SSL_H_
#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
struct _SSLString {
  size_t size;
  size_t length;
  char string[1];
};

#define SSL_BASE_POINTER(p_str)                                                \
  ((struct _SSLString*)(((char*)p_str) - offsetof(struct _SSLString, string)))

#define DEFAULT_RESIZE(size) size * 2
void ssl_free(char *str);
size_t ssl_strlen(char *str);
char *ssl_resize(char *str, size_t size);
char *ssl_strcpy(char *dest, const char *str);
char *ssl_strcat(char *dest, char *src);
char *ssl_addchar(char *str, char c);

#endif
