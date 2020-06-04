#include "parser.h"
#include "lexer.h"
#include "stretchy_buffer.h"
/* A subset of the token array */
struct Statement {
    unsigned int start;
    unsigned int end;
};
unsigned int statement_size (struct Statement s)
{
    return s.end - s.start + 1;
}

struct AST parse_tokens (struct Token* tokens)
{
    unsigned int index = 0;
    struct AST head = {0};
    struct AST** statements = NULL;
    while (index < sb_count (tokens)) {
        struct Statement s = parse__get_statement(tokens, &index);
        sb_push (statements, parse__statement (tokens, s));
    }

    head.type = AST_SCOPE;
    head.scope_data.statements = statements;
    return head;
}

struct Statement parse__get_statement (struct Token* restrict tokens, unsigned int* restrict index)
{
    unsigned int i = *index;
    struct Statement s = {0};
    while (i < sb_count (tokens) && tokens[i].value != STATEMENT_DELIM) {
        i++;
    }

    /* fill in statement info */
    s.start = *index;
    s.end = i - 1; /* don't care about the DELIM so the end if offset by 1 */

    *index = i + 1; /* offset by one to skip past the DELIM */
    return s;
}

struct AST* parse__statement (struct Token* t_arr, struct Statement s)
{
    struct AST* node = NULL;

    node = parse__possible_integer (t_arr, s);
    if (node) return node;

    node = parse__possible_identifier (t_arr, s);
    if (node) return node;

    return NULL;
}
struct AST* parse__possible_integer (struct Token* t_arr, struct Statement s)
{
    struct AST* node = NULL;
    if (statement_size (s)  == 1 &&
        t_arr[s.start].value == TOK_INTEGER) { /* INTEGER literal */
        node = parse__allocate_ast ();
        node->type = AST_INTEGER;
        node->int_data.value = atoi (t_arr[s.start].string);
    }
    return node;
}
struct AST* parse__possible_identifier (struct Token* t_arr, struct Statement s)
{
    struct AST* node = NULL;
    if (statement_size (s) == 1 &&
        t_arr[s.start].value == TOK_IDENTIFIER) {
        node = parse__allocate_ast ();
        node->type = AST_IDENTIFIER;
        node->id_data.id = t_arr[s.start].string;
    }
    return node;
}

struct AST* parse__allocate_ast ()
{
    struct AST a = {0};
    sb_push (ast_buffer, a);
    return &sb_last (ast_buffer);
}