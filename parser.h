#ifndef PARSER_H_
#define PARSER_H_
#include <stdbool.h>
#include "ast.h"
#include "lexer.h"
#include "queue.h"
#define STATEMENT_DELIM ';'
/* ****************
    External functions
   **************** */
struct Token;
ASTHandle parse_tokens (struct Token* tokens);


/* *********************
   Internal Functions
   ********************* */
/* Returns a statement struct containing the start and 
   end index of the next statement in the token array */
struct Statement parse__get_statement (struct Token* restrict tokens, unsigned int* restrict index);
/* Parsers *any* statement or substatement */
ASTHandle parse__statement (struct Token* t_arr, struct Statement s);

/* ========================================================
    Possible parsers
    Return a valid node if the statement is actually of X ast type
   ======================================================== */
ASTHandle parser__possible_integer (struct Token* t_arr, struct Statement s);
ASTHandle parser__possible_identifier (struct Token* t_arr, struct Statement s);
ASTHandle parser__possible_arithmetic (struct Token* t_arr, struct Statement s);
ASTHandle parser__possible_string (struct Token* t_arr, struct Statement s);

/* ===================
    Arithmetic helpers
   =================== */
bool parser__is_arithmetic_op (TValue v);
/* true if left is of greater operator precedence than right */
bool parser__greater_precedence (struct Token* left, struct Token* right);
/* true if left is of equal operator precedence than right */
bool parser__equal_precedence (struct Token* left, struct Token* right);

/* ==================
    Arithmetic parsing 
   ==================*/
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
/* converts a normal mathematical (infix) expression to something more workable */
struct TokenTailQ parser__convert_infix_to_postfix (struct TokenTailQ* expr_q);
/* converts a postfix expression gotten from convert_infix_to_postfix into an ast */
ASTHandle parser__convert_postfix_to_ast (struct TokenTailQ infix_q, unsigned int expr_size);


/* ==============
    Debug output
   ============== */
void parser__debug_print_queue (struct TokenTailQ* q);
#endif // PARSER_H_