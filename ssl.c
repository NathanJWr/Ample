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
#include "ncl.h"
#include "ssl.h"
/* Calls free on the base pointer of the ssl string */
void
ssl_free (char *str)
{
  free (SSL_BASE_POINTER (str));
}

/* Gets the strlen of an ssl string */
uint32_t
ssl_strlen (char *str)
{
  if (str)
    {
      struct _SSLString *s = SSL_BASE_POINTER (str);
      return s->length;
    }
  else
    {
      return 0;
    }
}
/* Create a ssl string from a cstring */
char *
ssl_strcpy (char *__restrict dest, const char *__restrict str)
{
  struct _SSLString *s = NULL;
  if (dest)
    {
      dest = ssl_resize (dest, strlen (str) + 1);
      s = SSL_BASE_POINTER (dest);
      strncpy (s->string, str, s->size);
      s->string[s->size - 1] = '\0';
      s->length += strlen (str);
    }
  else
    {
      uint32_t size = strlen (str) + 1;
      s = calloc (1, size + offsetof (struct _SSLString, string));
      s->size = size;
      s->length = strlen (str);

      strncpy (s->string, str, size);
      s->string[size - 1] = '\0';
    }
  return s->string;
}

/* Resize an ssl string to a desired size
 * Pointer to string may change, required to capture return value */
char *
ssl_resize (char *str, uint32_t size)
{
  struct _SSLString *n = SSL_BASE_POINTER (str);
  uint32_t orig_length = n->length;
  char *cp = NULL;

  /* Create a copy of the original string */
  cp = ssl_strcpy (cp, n->string);

  /* Reallocate memory to fit the new, bigger size specified */
  n = ncl_realloc (n, offsetof (struct _SSLString, string) + size);
  if (n)
    {
      n->size = size;
      n->length = orig_length;
      strncpy (n->string, cp, size);
      n->string[size-1] = '\0';
      ssl_free (cp);
      return n->string;
    }
  else
    {
      ssl_free (cp);
      return NULL;
    }
}

char *
ssl_strcat (char *__restrict dest, char *__restrict src)
{
  uint32_t size = 0;
  struct _SSLString *d = SSL_BASE_POINTER (dest);
  struct _SSLString *s = SSL_BASE_POINTER (src);

  assert (dest);
  assert (src);

  /* Resize the dest string to fit both src and dest */
  size = d->size + s->size;
  dest = ssl_resize (dest, size);
  d = SSL_BASE_POINTER (dest);
  d->length += s->length;
  strncat (dest, src, size);
  dest[size-1] = '\0';
  return dest;
}

char *
ssl_addchar (char *str, char c)
{
  if (str)
    {
      struct _SSLString *s = SSL_BASE_POINTER (str);

      if (s->length + 1 == s->size)
        {
          str = ssl_resize (str, DEFAULT_RESIZE (s->size));
        }
      str[SSL_BASE_POINTER (str)->length++] = c;
    }
  else
    {
      struct _SSLString *s
          = calloc (1, offsetof (struct _SSLString, string) +  (2 * sizeof (char)));
      s->string[0] = c;
      s->length = 1;
      s->size = 2;
      str = s->string;
    }

  return str;
}
