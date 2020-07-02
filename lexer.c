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
#include "array.h"
#include "lexer.h"
#include "ssl.h"
#include <ctype.h>
#include <stdio.h>

struct Token *
LexAll (char *fb)
{
  struct Token *tokens = NULL;
  size_t i = 0;
  char c = fb[i++];

  while (c != '\0')
    {
      struct Token token = { 0 };
      /* Remove any leading spaces */
      while (isspace (c))
        {
          c = fb[i++];
        }
      /* in case we hit eof after newline */
      if (c == '\0')
        break;
      if (isalpha (c))
        { /* IDENTIFIER */
          char *id = NULL;
          id = ssl_addchar (id, c);
          c = fb[i++]; /* get the next char to not add starting char twice */
          while (isalpha (c) || isdigit (c) || c == '_' || c == '-')
            {
              id = ssl_addchar (id, c);
              c = fb[i++];
            }
          i--;

          token.string = id;
          if (0 == strncmp ("true", id, ssl_strlen (id)))
            {
              token.value = TOK_BOOL;
            }
          else if (0 == strncmp ("false", id, ssl_strlen (id)))
            {
              token.value = TOK_BOOL;
            }
          else if (0 == strncmp ("if", id, ssl_strlen (id)))
            {
              token.value = TOK_IF;
            }
          else if (0 == strncmp("else", id, 4))
            {
              token.value = TOK_ELSE;
            }
          else if (0 == strncmp ("func", id, 4))
            {
              token.value = TOK_FUNC;
            }
          else
            {
              token.value = TOK_IDENTIFIER;
            }
        }
      else if (isdigit (c))
        { /* INTEGER */
          char *num = ssl_addchar (NULL, c);
          c = fb[i++];
          while (isdigit (c) || c == '.')
            {
              num = ssl_addchar (num, c);
              c = fb[i++];
            }
          i--;

          token.string = num;
          token.value = TOK_INTEGER;
        }
      else if (c == '"')
        { /* STRING */
          char *str = NULL;
          c = fb[i++];
          while (c != '"')
            {
              str = ssl_addchar (str, c);
              c = fb[i++];
            }
          token.string = str;
          token.value = TOK_STRING;
        }
      else if (c == '#')
        {
          /* comment, so skip the entire line */
          while (c != '\n')
            c = fb[i++];
          continue;
        }
      else
        { /* Any other kind of ASCII token */
          token.value = c;
        }
      c = fb[i++];
      ARRAY_PUSH (tokens, token);
    }
  return tokens;
}

void
TokenFreeAll (struct Token *tokens)
{
  size_t count = ARRAY_COUNT (tokens);
  size_t i;
  for (i = 0; i < count; i++)
    {
      if (tokens[i].string)
        ssl_free (tokens[i].string);
    }
  ARRAY_FREE (tokens);
}
