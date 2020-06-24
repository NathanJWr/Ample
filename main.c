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
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "mem_debug.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

char *
read_whole_file (FILE *f)
{
  size_t fsize;
  char* file;
  size_t size_read;

  fseek (f, 0, SEEK_END);
  fsize = ftell (f);
  fseek (f, 0, SEEK_SET);

  file = malloc (fsize + 1);
  size_read = fread (file, 1, fsize, f);
  assert (size_read <= fsize);

  file[size_read] = '\0';
  return file;
}

int
main (int argc, char **argv)
{
  if (argc > 1)
    {
      struct Token *tokens;
      ASTHandle ast_head;
      FILE *f = fopen (argv[1], "r");
      char *file = read_whole_file (f);
      fclose (f);

      tokens = LexAll (file);
      printf ("Memory after lexing...\n");
      MemDebugPrintInfo ();
      free (file);

      ast_head = ParseTokens (tokens);
      printf ("Memory after parsing...\n");
      MemDebugPrintInfo ();

      InterpreterStart (ast_head);
      printf ("Memory after interpreting...\n");
      MemDebugPrintInfo ();

      ast_free_buffer ();
      TokenFreeAll (tokens);

      printf ("Memory after program completion...\n");
      MemDebugPrintInfo ();
      MemDebugPrintLeaks ();
    }
  return 0;
}
