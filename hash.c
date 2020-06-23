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
size_t
hash_string (const char *s)
{
  const unsigned char *us = (const unsigned char *)s;
  size_t h = 0;
  while (*us != '\0')
    {
      h += h * HASH_MULTIPLIER + *us;
      us++;
    }
  return h;
}

size_t
hash_sizet(size_t x)
{
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;
}


bool32
string_compare (const char *key, const char *input)
{
  return (0 == strcmp (key, input));
}

bool32
int_compare (int key, int input)
{
  return (key == input);
}

bool32
sizet_compare (size_t key, size_t input)
{
  return (key == input);
}
