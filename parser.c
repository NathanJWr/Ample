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

struct AST* parse_tokens (struct Token* tokens)
{
    unsigned int index = 0;
    unsigned int head = parse__allocate_ast ();
    unsigned int* statements = NULL;
    while (index < sb_count (tokens)) {
        struct Statement s = parse__get_statement(tokens, &index);
        sb_push (statements, parse__statement (tokens, s));
    }

    struct AST* h = parse__get_ast (head);
    h->type = AST_SCOPE;
    h->scope_data.statements = statements;
    return h;
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

unsigned int parse__statement (struct Token* t_arr, struct Statement s)
{
    unsigned int node = 0;
    node = parse__possible_integer (t_arr, s);
    if (node) return node;

    node = parse__possible_identifier (t_arr, s);
    if (node) return node;

    return 0;
}
unsigned int parse__possible_integer (struct Token* t_arr, struct Statement s)
{
    unsigned int node = 0;
    if (statement_size (s)  == 1 &&
        t_arr[s.start].value == TOK_INTEGER) { /* INTEGER literal */
        node = parse__allocate_ast ();
        struct AST* n = parse__get_ast (node);
        n->type = AST_INTEGER;
        n->int_data.value = atoi (t_arr[s.start].string);
    }
    return node;
}
unsigned int parse__possible_identifier (struct Token* t_arr, struct Statement s)
{
    unsigned int node = 0;
    if (statement_size (s) == 1 &&
        t_arr[s.start].value == TOK_IDENTIFIER) {
        node = parse__allocate_ast ();
        struct AST* n = parse__get_ast (node);
        n->type = AST_IDENTIFIER;
        n->id_data.id = t_arr[s.start].string;
    }
    return node;
}

unsigned int parse__allocate_ast ()
{
    struct AST a = {0};
    sb_push (ast_buffer, a);
    return sb_count(ast_buffer) - 1;
}

void free_ast_buffer ()
{
    for (unsigned int i = 0; i < sb_count (ast_buffer); i++) {
        if (ast_buffer[i].type == AST_SCOPE) {
            sb_free (ast_buffer[i].scope_data.statements);
        }
    }
    sb_free (ast_buffer);
}

struct AST* parse__get_ast(unsigned int index)
{
    return &ast_buffer[index];
}