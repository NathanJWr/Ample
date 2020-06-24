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
#include "ast.h"
#include "lexer.h"
#include "queue.h"
#include "stack.h"
#define STATEMENT_DELIM ';'
struct Token;
ASTHandle ParseTokens(struct Token *tokens);

/* ******************
 * internal functions
 * ****************** */
/* A subset of the token array */
struct Statement {
  unsigned int start;
  unsigned int end;
};
/* Returns a statement struct containing the start and
   end index of the next statement in the token array */
struct Statement get_statement(struct Token *__restrict tokens,
                                      unsigned int *__restrict index);
/* returns the number of tokens that make up a statement */
unsigned int statement_size(struct Statement s);
/* Parsers *any* statement or substatement */
ASTHandle parse_statement(struct Token *t_arr, struct Statement s);

/* ========================================================
    Possible parsers
    Return a valid node if the statement is actually of X ast type
   ======================================================== */
ASTHandle parse_possible_integer(struct Token *t_arr, struct Statement s);
ASTHandle parse_possible_identifier(struct Token *t_arr, struct Statement s);
ASTHandle parse_possible_arithmetic(struct Token *t_arr, struct Statement s);
ASTHandle parse_possible_string(struct Token *t_arr, struct Statement s);
ASTHandle parse_possible_assignment(struct Token *t_arr, struct Statement s);
ASTHandle parse_possible_if_statement (struct Token *t_arr, struct Statement s);
ASTHandle parse_possible_bool (struct Token *t_arr, struct Statement s);
ASTHandle parse_possible_equality (struct Token *t_arr, struct Statement s_indexes);
ASTHandle parse_scope (struct Token *t_arr, struct Statement s);

/* ===================
    Arithmetic helpers
   =================== */
bool32 is_arithmetic_op(TValue v);
/* true if left is of greater operator precedence than right */
bool32 greater_precedence(struct Token *left, struct Token *right);
/* true if left is of equal operator precedence than right */
bool32 equal_precedence(struct Token *left, struct Token *right);
/* checks if there are any tokens that don't make sense in an arithmetic statement */
bool32 contains_invalid_arithmetic_token (struct Token *t_arr, struct Statement s);

/* ==================
    Arithmetic parsing
   ==================*/
STACK_DECLARATION(TokenStack, struct Token *);
QUEUE_DECLARATION(TokenQueue, struct Token *);
STACK_DECLARATION(ASTHandleStack, ASTHandle);
/* converts a normal mathematical (infix) expression to something more workable
 */
QUEUE(TokenQueue) convert_infix_to_postfix(QUEUE(TokenQueue) * expr_q);
/* converts a postfix expression gotten from convert_infix_to_postfix into an
 * ast */
ASTHandle convert_postfix_to_ast(QUEUE(TokenQueue) * infix_q);

/* ==============
    Debug output
   ============== */
void debug_print_queue(QUEUE(TokenQueue) * q);
void debug_print_statement(struct Token* t_arr, struct Statement s);

/* ************************************************************************* */

#endif
