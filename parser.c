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
#include "parser.h"
#include "array.h"
#include "ast.h"
#include "lexer.h"
#include "queue.h"

unsigned int
statement_size (struct Statement s)
{
  return s.end - s.start + 1;
}

ASTHandle
ParseTokens (struct Token *tokens)
{
  unsigned int index = 0;
  ASTHandle head = ast_get_node_handle ();
  ASTHandle *statements = NULL;
  struct AST *h;

  while (index < ARRAY_COUNT (tokens))
    {
      struct Statement s = get_statement (tokens, &index);
      ARRAY_PUSH (statements, parse_statement (tokens, s));
    }

  h = ast_get_node (head);
  h->type = AST_SCOPE;
  h->d.scope_data.statements = statements;
  return head;
}

struct Statement
get_statement (struct Token *__restrict tokens,
                      unsigned int *__restrict index)
{
  unsigned int i = *index;
  struct Statement s = { 0 };

  unsigned int scoped = 0;
  while (i < ARRAY_COUNT (tokens))
    {
      if (tokens[i].value == '{')
        scoped++;
      if (tokens[i].value == '}')
        scoped--;
      if ((tokens[i].value == STATEMENT_DELIM || tokens[i].value == '}') && scoped == 0)
        break;
      i++;
    }

  /* fill in statement info */
  s.start = *index;
  s.end = i; /* don't care about the DELIM so the end if offset by 1 */

  *index = i + 1; /* offset by one to skip past the DELIM */
  return s;
}

ASTHandle
parse_statement (struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  node = parse_possible_if_statement (t_arr, s);
  if (node)
    return node;

  node = parse_possible_assignment (t_arr, s);
  if (node)
    return node;

  node = parse_possible_integer (t_arr, s);
  if (node)
    return node;

  node = parse_possible_identifier (t_arr, s);
  if (node)
    return node;

  node = parse_possible_arithmetic (t_arr, s);
  if (node)
    return node;

  node = parse_possible_string (t_arr, s);
  if (node)
    return node;

  node = parse_possible_bool (t_arr, s);
  if (node)
    return node;

  node = parse_possible_equality (t_arr, s);
  if (node)
    return node;

  printf ("Invalid statement: \n");
  debug_print_statement (t_arr, s);
  exit (1);
}
ASTHandle
parse_possible_equality (struct Token *t_arr, struct Statement s_indexes)
{
  ASTHandle node = 0;
  /* FORMAT
   * Left_Expr == Right_Expr */
  unsigned int i;
  for (i = s_indexes.start; i < s_indexes.end - 1; i++)
    {
      if (t_arr[i].value == '=' && t_arr[i+1].value == '=')
        {
          struct Statement left_statement, right_statement;
          ASTHandle left_handle, right_handle;
          struct AST *equality_ast;

          /* split the statement into two, left expr and right */
          left_statement.start = s_indexes.start;
          left_statement.end = i - 1;
          right_statement.start = i + 2;
          right_statement.end = s_indexes.end;

          /* parse the left and right statements into ast nodes */
          left_handle = parse_statement (t_arr, left_statement);
          right_handle = parse_statement (t_arr, right_statement);

          /* create a new ast node for the equality statement */
          node = ast_get_node_handle ();
          equality_ast = ast_get_node (node);
          equality_ast->type = AST_EQUALITY;
          equality_ast->d.equality_data.left = left_handle;
          equality_ast->d.equality_data.right = right_handle;

          /* don't need to keep moving through the for loop */
          return node;
        }
    }
  return node;
}
ASTHandle
parse_possible_bool(struct Token* t_arr, struct Statement s)
{
  ASTHandle node = 0;
  if ((statement_size(s) >= 2 &&
      t_arr[s.start].value == TOK_BOOL &&
      t_arr[s.start + 1].value == STATEMENT_DELIM) ||
      (statement_size (s) == 1 && t_arr[s.start].value == TOK_BOOL))
    {
      struct AST *n = NULL;
      const struct Token *tok = &t_arr[s.start];
      node = ast_get_node_handle ();
      n = ast_get_node (node);

      n->type = AST_BOOL;
      if (0 == strncmp (tok->string, "true", 4))
        n->d.bool_data.value = true;
      else if (0 == strncmp (tok->string, "false", 5))
        n->d.bool_data.value = false;
    }
  return node;
}
ASTHandle
parse_possible_if_statement (struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  if (t_arr[s.start].value == TOK_IF)
    {
      /* statement inside parens should evaluate to a bool */
      /* there should be exactly 1 expression inside if parens */
      struct AST *n = NULL;
      ASTHandle expr = 0;
	  ASTHandle scope_if_true = 0;
      struct Statement paren_statement;
      struct Statement scope_statement;
      unsigned int i = s.start + 2; /* skip over the '(' character */
      while (t_arr[i].value != ')')
        i++;
      paren_statement.start = s.start + 2;
      paren_statement.end = i-1;
      expr = parse_statement (t_arr, paren_statement); 

      /* create if ast node */
      node = ast_get_node_handle();
 
      /* find the start of the if scope */
      while (t_arr[i].value != '{')
        i++;
      scope_statement.start = i;
      scope_statement.end = s.end;
      scope_if_true = parse_scope (t_arr, scope_statement);
	  
      n = ast_get_node (node);
      n->d.if_data.scope_if_true = scope_if_true;
      n->d.if_data.expr = expr;
      n->type = AST_IF;
    }
  return node;
}
ASTHandle
parse_scope(struct Token* t_arr, struct Statement s)
{
  ASTHandle handle = ast_get_node_handle ();
  struct AST *scope = NULL;
  unsigned int index = s.start + 1; /* skip '{' */
  ASTHandle *statements = NULL;
  while (t_arr[index].value != '}')
    {
      struct Statement statement_indexes = get_statement(t_arr, &index);
      ASTHandle statement = parse_statement (t_arr, statement_indexes);
      ARRAY_PUSH (statements, statement);
    }
  
  scope = ast_get_node (handle);
  scope->type = AST_SCOPE;
  scope->d.scope_data.statements = statements;
  return handle;
}

ASTHandle
parse_possible_integer (struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  if ((statement_size (s) >= 2 && 
      t_arr[s.start].value == TOK_INTEGER &&
      t_arr[s.start + 1].value == STATEMENT_DELIM) ||
      (statement_size (s) == 1 &&
       t_arr[s.start].value == TOK_INTEGER))
    { /* INTEGER literal */
      struct AST *n;

      node = ast_get_node_handle ();
      n = ast_get_node (node);
      n->type = AST_INTEGER;
      n->d.int_data.value = atoi (t_arr[s.start].string);
    }
  return node;
}

bool32
is_arithmetic_op (TValue v)
{
  return (v == '+' || v == '-' || v == '/' || v == '*');
}

bool32
greater_precedence (struct Token *left, struct Token *right)
{
  if ('*' == left->value)
    {
      if (right->value == '+' || right->value == '-')
        return true;
    }
  else if ('/' == left->value)
    {
      if (right->value == '+' || right->value == '-')
        return true;
    }
  return false;
}
bool32
equal_precedence (struct Token *left, struct Token *right)
{
  if (('+' == left->value || '-' == left->value)
      && ('+' == right->value || '-' == right->value))
    {
      return true;
    }
  if (('*' == left->value || '/' == left->value)
      && ('*' == right->value || '/' == right->value))
    {
      return true;
    }
  return false;
}

QUEUE (TokenQueue)
convert_infix_to_postfix (QUEUE (TokenQueue) * expr_q)
{
  STACK (TokenStack) s;
  QUEUE (TokenQueue) q;
  STACK_STRUCT_INIT (TokenStack, &s, struct Token *, 10);
  QUEUE_STRUCT_INIT (TokenQueue, &q, struct Token *, 10);
  while (!QUEUE_EMPTY (expr_q))
    {
      struct Token *n = QUEUE_FRONT (expr_q);
      QUEUE_POP (expr_q);

      if (n->value == TOK_INTEGER || n->value == TOK_IDENTIFIER || n->value == TOK_STRING)
        {
          QUEUE_PUSH (&q, n);
        }
      if (is_arithmetic_op (n->value))
        {
          if (!STACK_EMPTY (&s))
            {
              struct Token *top_token = STACK_FRONT (&s);
              while ((greater_precedence (top_token, n))
                     || ((equal_precedence (top_token, n))
                         && (top_token->value != ')')))
                {
                  STACK_POP (&s);
                  QUEUE_PUSH (&q, top_token);

                  if (STACK_EMPTY (&s))
                    break;
                  top_token = STACK_FRONT (&s);
                }
            }
          STACK_PUSH (&s, n);
        }
      if (n->value == '(')
        {
          STACK_PUSH (&s, n);
        }
      if (n->value == ')')
        {
          struct Token *tn = STACK_FRONT (&s);
          STACK_POP (&s);
          while (tn->value != '(')
            {
              QUEUE_PUSH (&q, tn);
              tn = STACK_FRONT (&s);
              STACK_POP (&s);
            }
        }
      if (n->value == STATEMENT_DELIM)
        break;
    }
  while (!STACK_EMPTY (&s))
    {
      struct Token *tn = STACK_FRONT (&s);
      STACK_POP (&s);
      QUEUE_PUSH (&q, tn);
    }
  STACK_FREE (&s, TokenStack);
  QUEUE_FREE (expr_q, TokenQueue);
  return q;
}
ASTHandle
convert_postfix_to_ast (QUEUE (TokenQueue) * postfix_q)
{
  STACK (ASTHandleStack) s;
  ASTHandle return_handle;
  STACK_STRUCT_INIT (ASTHandleStack, &s, ASTHandle, 10);
  while (!QUEUE_EMPTY (postfix_q))
    {
      struct Token *n = QUEUE_FRONT (postfix_q);
      QUEUE_POP (postfix_q);

      if (n->value == TOK_INTEGER)
        {
          ASTHandle ast_handle = ast_get_node_handle ();
          struct AST *integer_ast = ast_get_node (ast_handle);
          integer_ast->type = AST_INTEGER;
          integer_ast->d.int_data.value = atoi (n->string);

          STACK_PUSH (&s, ast_handle);
        }
      else if (n->value == TOK_IDENTIFIER)
        {
          ASTHandle ast_handle = ast_get_node_handle ();
          struct AST *id_ast = ast_get_node (ast_handle);
          id_ast->type = AST_IDENTIFIER;
          id_ast->d.id_data.id = n->string;

          STACK_PUSH (&s, ast_handle);
        }
      else if (n->value == TOK_STRING)
        {
          ASTHandle ast_handle = ast_get_node_handle ();
          struct AST *str = ast_get_node (ast_handle);
          str->type = AST_STRING;
          str->d.str_data.str = n->string;

          STACK_PUSH (&s, ast_handle);
        }
      else if (is_arithmetic_op (n->value))
        {
          ASTHandle left, right, ast_handle;
          struct AST *op;

          /* Remove the first two elements from the stack.
             They will be the left and right params of a binary op */
          left = STACK_FRONT (&s);
          STACK_POP (&s);
          right = STACK_FRONT (&s);
          STACK_POP (&s);
          ast_handle = ast_get_node_handle ();

          /* Fill out information of binary op */
          op = ast_get_node (ast_handle);
          op->type = AST_BINARY_OP;
          op->d.bop_data.left = left;
          op->d.bop_data.right = right;
          op->d.bop_data.op = n->value;
          /* Insert the operation, instead of the left and right values */
          STACK_PUSH (&s, ast_handle);
        }
    }
  return_handle = STACK_FRONT (&s);
  STACK_FREE (&s, ASTHandleStack);
  QUEUE_FREE (postfix_q, TokenQueue);
  return return_handle;
}

ASTHandle
parser__arithmetic (struct Token *t_arr, struct Statement s)
{
  /* storage necessary to put tokens into a queue/stack
     no more memory should be allocated for this process */
  QUEUE (TokenQueue) expr_q = { 0 };
  QUEUE (TokenQueue) postfix = { 0 };
  unsigned int i;
  ASTHandle op;

  QUEUE_STRUCT_INIT (TokenQueue, &expr_q, struct Token *, 10);
  for (i = s.start; i <= s.end; i++)
    {
      QUEUE_PUSH (&expr_q, t_arr + i);
    }
  postfix = convert_infix_to_postfix (&expr_q);
  op = convert_postfix_to_ast (&postfix);
  return op;
}

void debug_print_queue (QUEUE (TokenQueue) * q)
{
  struct Token *np;
  int i;

  for (i = q->head; i <= q->tail; i++)
    {
      np = q->mem[i];
      if (np->value < 128)
        printf ("Value: %c ", np->value);
      else
        printf ("Value: %d ", np->value);
      if (np->string)
        printf ("String: %s", np->string);
      printf ("\n");
    }
}

bool32
contains_invalid_arithmetic_token (struct Token *t_arr, struct Statement s)
{
  unsigned int i;
  for (i = s.start; i < s.end; i++)
    {
      if (t_arr[i].value == '=')
        return true;
    }
  return false;
}

ASTHandle
parse_possible_arithmetic (struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  /* Expression Format:
   * a + b ...
   * (a / b ... */
  if (statement_size (s) >= 2
      && (t_arr[s.start].value == TOK_INTEGER
          || t_arr[s.start].value == TOK_IDENTIFIER
          || t_arr[s.start].value == TOK_STRING)
      && is_arithmetic_op (t_arr[s.start + 1].value)
      && !contains_invalid_arithmetic_token (t_arr, s))
    {
      node = parser__arithmetic (t_arr, s);
    }
  return node;
}

ASTHandle
parse_possible_identifier (struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  if ((statement_size (s) == 1 && t_arr[s.start].value == TOK_IDENTIFIER) ||
      (statement_size (s) == 2 && t_arr[s.start].value == TOK_IDENTIFIER &&
       t_arr[s.start + 1].value == STATEMENT_DELIM))
    {
      struct AST *n = NULL;
      node = ast_get_node_handle ();
      n = ast_get_node (node);
      n->type = AST_IDENTIFIER;
      n->d.id_data.id = t_arr[s.start].string;
    }
  return node;
}

ASTHandle
parse_possible_string (struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  if (statement_size (s) >= 2 &&
      t_arr[s.start].value == TOK_STRING &&
      t_arr[s.start + 1].value == STATEMENT_DELIM)
    {
      struct AST *n = NULL;
      node = ast_get_node_handle ();
      n = ast_get_node (node);
      n->type = AST_STRING;
      n->d.str_data.str = t_arr[s.start].string;
    }
  return node;
}

ASTHandle
parse_possible_assignment (struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  /* VAR = EXPR */
  if (statement_size (s) >= 3 && t_arr[s.start].value == TOK_IDENTIFIER
      && t_arr[s.start + 1].value == '=' && t_arr[s.start + 2].value != '=')
    {
      struct Statement sub_statement;
      struct AST ast;
      struct AST *n = NULL;
      node = ast_get_node_handle ();

      sub_statement.start = s.start + 2;
      sub_statement.end = s.end;
      ast.type = AST_ASSIGNMENT;
      ast.d.asgn_data.var = t_arr[s.start].string;
      ast.d.asgn_data.expr = parse_statement (t_arr, sub_statement);

      n = ast_get_node (node);
      *n = ast;
    }
  return node;
}
void
debug_print_statement(struct Token* t_arr, struct Statement s)
{
  unsigned int i;
  for (i = s.start; i <= s.end; i++)
    {
      if (t_arr[i].value <= 125)
        printf ("Token %d: %c\n", i - s.start, t_arr[i].value);
      else
        printf ("Token %d: %s\n", i - s.start, t_arr[i].string);
    }
}
