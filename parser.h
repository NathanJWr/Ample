#ifndef PARSER_H_
#define PARSER_H_
#include <stdbool.h>
#include "ast.h"
#include "lexer.h"
#include "queue.h"
#define STATEMENT_DELIM ';'
/* external functions */
struct Token;
struct AST* parse_tokens (struct Token* tokens);


/* internal functions */
struct Statement parse__get_statement (struct Token* restrict tokens, unsigned int* restrict index);
ASTHandle parse__statement (struct Token* t_arr, struct Statement s);

ASTHandle parser__possible_integer (struct Token* t_arr, struct Statement s);
ASTHandle parser__possible_identifier (struct Token* t_arr, struct Statement s);
ASTHandle parser__possible_arithmetic (struct Token* t_arr, struct Statement s);

bool parser__is_arithmetic_op (TValue v);
bool parser__greater_precedence (struct Token* left, struct Token* right);
bool parser__equal_precedence (struct Token* left, struct Token* right);

/* arithmetic parsing */
struct TokenNode {
    struct Token* tok;
    STAILQ_ENTRY(TokenNode) tqentries; /* entries for queue */
    SLIST_ENTRY(TokenNode) slentries; /* entries for stack */
};
STAILQ_HEAD (TokenTailQ, TokenNode);
SLIST_HEAD (TokenStack, TokenNode);
struct ASTNode {
    ASTHandle ast;
    SLIST_ENTRY(ASTNode) slentries; /* entries for stack */
};
SLIST_HEAD (ASTHandleStack, ASTNode);

struct TokenTailQ parser__convert_infix_to_postfix (struct TokenTailQ* expr_q);
ASTHandle parser__convert_postfix_to_ast (struct TokenTailQ infix_q, unsigned int expr_size);


/* debug output */
void parser__debug_print_queue (struct TokenTailQ* q);
#endif // PARSER_H_