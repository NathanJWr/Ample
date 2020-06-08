#include "ssl.h"
#include "array.h"
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
            if (0 == strncmp ("true", id, ssl_strlen(id))) {
                token.value = TOK_BOOL;
            } else if (0 == strncmp ("false", id, ssl_strlen (id))) {
                token.value = TOK_BOOL;
            } else {
                token.value = TOK_IDENTIFIER;
            }
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
        } else if (c == '"') { /* STRING */
            char* str = NULL;
            c = fb[i++];
            while (c != '"') {
                str = ssl_addchar (str, c);
                c = fb[i++];
            }
            token.string = str;
            token.value = TOK_STRING;
        } else { /* Any other kind of ASCII token */
            token.value = c;
        }
        c = fb[i++];
        ARRAY_PUSH(tokens, token);
    }
    return tokens;
}

void token_free_all (struct Token* tokens)
{
    uint32_t count = ARRAY_COUNT (tokens);
    for (uint32_t i = 0; i < count; i++) {
        if (tokens[i].string)
            ssl_free (tokens[i].string);
    }
    ARRAY_FREE (tokens);
}