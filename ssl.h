/*
    This file is part of Ample.

    Ample is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef SSL_H_
#define SSL_H_
#include <string.h>
#include <bsd/string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
struct _SSLString {
    uint32_t size;
    uint32_t length;
    char string[];
};

// Access elements

#define SSL_BASE_POINTER(p_str) ((struct _SSLString*) (p_str - sizeof(struct _SSLString)))
#define DEFAULT_RESIZE(size) size*2
void ssl_free (char* str);
uint32_t ssl_strlen (char* str);
char* ssl_resize (char* str, uint32_t size);
char* ssl_strcpy (char* dest, char* str);
char* ssl_strcat (char* dest, char* src);
char* ssl_addchar (char* str, char c);


#endif // SSL_H_