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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

char* read_whole_file (FILE* f)
{
    fseek (f, 0, SEEK_END);
    long fsize = ftell (f);
    fseek (f, 0, SEEK_SET);

    char* file = malloc(fsize + 1);
    size_t size_read = fread (file, 1, fsize, f);
    assert (size_read == fsize);

    file[fsize] = '\0';
    return file;
}

int main (int argc, char** argv) {
    if (argc > 1) {
        FILE* f = fopen(argv[1], "r");
        char* file = read_whole_file (f);
        fclose (f);

        struct Token* tokens = lex_all (file);
        ASTHandle ast_head = parse_tokens (tokens);

        ast_free_buffer ();
        token_free_all (tokens);
        free (file);
    }
}