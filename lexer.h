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
#ifndef LEXER_H_
#define LEXER_H_
#include <stdio.h>
/* starts with the last ascii value */
/* ascii values can be represented as tokens just by their value */
typedef enum {
  TOK_IDENTIFIER = 127,
  TOK_INTEGER = 128,
  TOK_BOOL = 129,
  TOK_STRING = 130,
  TOK_IF = 131
} TValue;
struct Token {
  TValue value;
  char *string; /* ssl managed string */
};

void token_free_all(struct Token *tokens);
struct Token *lex_all(char *fb);

#endif
