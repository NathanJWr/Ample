#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lexer.h"
#include "parser.h"

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
        struct AST tree = parse_tokens (tokens);
        token_free_all (tokens);
        free (file);
    }
}