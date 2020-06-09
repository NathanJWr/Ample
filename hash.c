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
#include "hash.h"
#include "ssl.h"
uint64_t hash_string (const char* s)
{
    const unsigned char* us = (const unsigned char*) s;
    uint64_t h = 0;
    while (*us != '\0') {
        h += h * HASH_MULTIPLIER + *us;
        us++;
    }
    return h;
}

DICT_DECLARATION (String, const char*, int);
int main () {
    DICT(String) s = {0};
    DICT_INIT (&s, hash_string, 1);

    char* vars [] = {
        "abc",
        "123",
        "bcav",
        "321",
        "cac",
        "pac",
    };

    for (int i = 0; i < 6; i++) {
        DICT_INSERT (String, &s, vars[i], i + 20);
    }


    int b = 0;
}

