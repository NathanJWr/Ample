#ifndef LEXER_H_
#define LEXER_H_
#include <stdio.h>
/* starts with the last ascii value */
/* ascii values can be represented as tokens just by their value */
typedef enum {
    TOK_IDENTIFIER = 127,
    TOK_INTEGER = 128,
    TOK_BOOL = 129,
} TValue;
struct Token {
    TValue value;
    char* string; /* ssl managed string */
};

void token_free_all (struct Token* tokens);
struct Token* lex_all (char* fb);

#endif // LEXER_H_