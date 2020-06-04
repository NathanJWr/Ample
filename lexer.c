#include "ssl.h"
#include "stretchy_buffer.h"
#include "lexer.h"
#include <stdio.h>
#include <ctype.h>

struct Token* lex_all (char* fb)
{
    struct Token* tokens = NULL;
    size_t i = 0;
    char c = fb[i++];

    while (c != '\0') {
        struct Token token = {0};
        /* Remove any leading spaces */
        while (isspace (c)) {
            c = fb[i++];
        }
        if (isalpha (c)) { /* IDENTIFIER */
            char* id = NULL;
            id = ssl_addchar(id, c);
            c = fb[i++]; /* get the next char to not add starting char twice */
            while (isalpha (c) || isdigit (c) || c == '_' || c == '-') {
                id = ssl_addchar (id, c);
                c = fb[i++];
            }
            i--;

            token.string = id;
            token.value = TOK_IDENTIFIER;
        } else if (isdigit (c) && c != '0') { /* INTEGER */
            char* num = ssl_addchar (NULL, c);
            c = fb[i++];
            while (isdigit (c)) {
                num = ssl_addchar (num, c);
                c = fb[i++];
            }
            i--;

            token.string = num;
            token.value = TOK_INTEGER;
        } else {
            token.value = c;
        }
        c = fb[i++];
        sb_push(tokens, token);
    }
    return tokens;
}

void token_free_all (struct Token* tokens)
{
    uint32_t count = sb_count (tokens);
    for (uint32_t i = 0; i < count; i++) {
        if (tokens[i].string)
            ssl_free (tokens[i].string);
    }
    sb_free (tokens);
}