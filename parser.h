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
#ifndef PARSER_H_
#define PARSER_H_
#include <stdbool.h>
#include "ast.h"
#include "lexer.h"
#include "queue.h"
#include "stack.h"
#define STATEMENT_DELIM ';'
/* ****************
    External functions
   **************** */
struct Token;
ASTHandle parse_tokens (struct Token* tokens);


/* *********************
   Internal Functions
   ********************* */

/* ======================
    Statement declaratins
   ====================== */
/* A subset of the token array */
struct Statement {
    unsigned int start;
    unsigned int end;
};
/* returns the number of tokens that make up a statement */
unsigned int statement_size (struct Statement s);

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
ASTHandle parser__possible_assignment (struct Token* t_arr, struct Statement s);

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
STACK_DECLARATION (TokenStack, struct Token*);
QUEUE_DECLARATION (TokenQueue, struct Token*);
STACK_DECLARATION (ASTHandleStack, ASTHandle);
/* converts a normal mathematical (infix) expression to something more workable */
QUEUE (TokenQueue) parser__convert_infix_to_postfix (QUEUE (TokenQueue)* expr_q);
/* converts a postfix expression gotten from convert_infix_to_postfix into an ast */
ASTHandle parser__convert_postfix_to_ast (QUEUE (TokenQueue)* infix_q, unsigned int expr_size);


/* ==============
    Debug output
   ============== */
void parser__debug_print_queue (QUEUE (TokenQueue)* q);
#endif // PARSER_H_