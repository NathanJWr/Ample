#include <stdbool.h>
#include "../test_helper.h"
#include "../lexer.c"

bool test_lexer_identifier()
{
    char *id = "abcd123";
    struct Token *tokens = lex_all(id);
    EXPECT(sb_count(tokens) == 1);
    EXPECT(tokens[0].value == TOK_IDENTIFIER);
    EXPECT(0 == strcmp(tokens[0].string, id));

    token_free_all (tokens);
    return true;
}

bool test_lexer_integer()
{
    char *str = "1235";
    struct Token *tokens = lex_all(str);
    EXPECT(sb_count(tokens) == 1);
    EXPECT(tokens[0].value == TOK_INTEGER);
    EXPECT(0 == strcmp(tokens[0].string, str));

    token_free_all (tokens);
    return true;
}

bool test_lexer_integer_and_identifier()
{
    char *str = "abcd 123";
    struct Token *tokens = lex_all (str);
    EXPECT (sb_count(tokens) == 2);

    EXPECT (tokens[0].value == TOK_IDENTIFIER);
    EXPECT (0 == strcmp (tokens[0].string, "abcd"));

    EXPECT (tokens[1].value == TOK_INTEGER);
    EXPECT (0 == strcmp (tokens[1].string, "123"));

    token_free_all (tokens);
    return true;
}

bool test_lexer_bools ()
{
    char* t = "true";
    struct Token *tokens = lex_all (t);
    EXPECT (sb_count(tokens) == 1);
    EXPECT (tokens[0].value == TOK_BOOL);
    EXPECT (0 == strcmp (tokens[0].string, t));
    token_free_all (tokens);

    char* f = "false";
    tokens = lex_all (f);
    EXPECT (sb_count(tokens) == 1);
    EXPECT (tokens[0].value == TOK_BOOL);
    EXPECT (0 == strcmp (tokens[0].string, f));
    token_free_all (tokens);

    return true;
}

bool test_lexer_bools_mangled ()
{
    char* str = "true1234";
    struct Token *tokens = lex_all (str);
    EXPECT (sb_count (tokens) == 1);
    EXPECT (tokens[0].value == TOK_IDENTIFIER);
    EXPECT (0 == strcmp (tokens[0].string, str));

    token_free_all (tokens);
    return true;
}

int main()
{
    TRY (test_lexer_identifier);
    TRY (test_lexer_integer);
    TRY (test_lexer_integer_and_identifier);
    TRY (test_lexer_bools);
    TRY (test_lexer_bools_mangled);
}
