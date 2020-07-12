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

unsigned int global_statement_index;
ASTHandle
ParseTokens (struct Token *tokens)
{
  ASTHandle head = ast_get_node_handle ();
  ASTHandle *statements = NULL;
  struct AST *h;

  while (global_statement_index < ARRAY_COUNT (tokens) - 1)
    {
      struct Statement s = get_statement (tokens, &global_statement_index);
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
      if ((tokens[i].value == STATEMENT_DELIM || tokens[i].value == '}') && 
           scoped == 0)
        break;
      i++;
    }

  /* fill in statement info */
  s.start = *index;
  s.end = i; /* don't care about the DELIM so the end if offset by 1 */

  *index = i + 1; /* offset by one to skip past the DELIM */

  #ifdef PARSER_DEBUG
  printf ("DEGUG Statement: \n");
  debug_print_statement (tokens, s);
  #endif
  return s;
}

ASTHandle
parse_statement (struct Token *t_arr, struct Statement s)
{
  /* the "biggest" kinds of statements should go first
   * that way we pick the biggest statement (like an if statement)
   * and then progressively parse smaller and smaller blocks */
  ASTHandle node = 0;
  node = parse_possible_if_statement (t_arr, s);
  if (node)
    return node;
  node = parse_possible_function (t_arr, s);
  if (node)
    return node;

  node = parse_possible_function_call (t_arr, s);
  if (node)
    return node;

  node = parse_possible_equality (t_arr, s);
  if (node)
    return node;

  node = parse_possible_assignment (t_arr, s);
  if (node)
    return node;

  node = parse_possible_list (t_arr, s);
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

  if (is_arithmetic_op (t_arr[s.start].value))
    {
      node = ast_get_node_handle ();
      struct AST *ast = ast_get_node (node);
      ast->d.op_data.op = t_arr[s.start].value;
      ast->type = AST_OP;
      return node;
    }


  printf ("Invalid statement: \n");
  debug_print_statement (t_arr, s);
  exit (1);
}

ASTHandle
parse_possible_list(struct Token *t_arr, struct Statement s)
{
  /* [ ... ]
   * [ A, B, C]
   * [] */
  if (statement_size (s) >= 2 &&
      t_arr[s.start].value == '[')
    {
      unsigned int end_index;
      return parse_list (t_arr, s, &end_index);
    }

  return 0;
}

ASTHandle
parse_list (struct Token *restrict t_arr,
            struct Statement s,
            unsigned int *restrict end_index)
{
  ASTHandle node = 0;
  struct AST *n = NULL;
  ASTHandle *items = NULL;
  unsigned int end = s.start + 1; /* skip '[' */
  while (1)
    {
      /* parse a comma separated argument */
      unsigned int start = end;
      while (t_arr[end].value != ',' && t_arr[end].value != ']')
        {
          if (t_arr[end].value == '[') /* inner list */
            {
              struct Statement inner_list = { end, s.end };
              ARRAY_PUSH (items, parse_list (t_arr, inner_list, &end));
              start = ++end;
              continue;
            }
          end++;
        }
      if (end-start >= 1)
        {
          struct Statement item_statement;
          item_statement.start = start;
          item_statement.end = end-1; /* ignore ',' */
          ARRAY_PUSH (items, parse_statement (t_arr, item_statement));
        }
      if (t_arr[end].value == ']')
        {
          *end_index = end;
          break;
        }
      else
        {
          ++end; /* skip over ',' */
        }
    }
  node = ast_get_node_handle ();
  n = ast_get_node (node);
  n->type = AST_LIST;
  n->d.list_data.items = items;

  return node;
}

ASTHandle
parse_possible_function_call(struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  /* lowerst possible size is 3
   * i.e. "func()" is an identifier and
   * the two paren tokens */
  if (statement_size (s) >= 3)
    {
      if (t_arr[s.start].value == TOK_IDENTIFIER &&
          t_arr[s.start + 1].value == '(')
        {
          struct AST *func_call;
          ASTHandle *args =
            parse_arguments_surrounded_by_parens (t_arr, s.start + 1);

          node = ast_get_node_handle ();
          func_call = ast_get_node (node);
          func_call->type = AST_FUNC_CALL;
          func_call->d.func_call_data.name = t_arr[s.start].string;
          func_call->d.func_call_data.args = args;
        }
    }
  return node;
}

ASTHandle *
parse_arguments (struct Token *t_arr, struct Statement s)
{
  unsigned int i;
  unsigned int start_statement = s.start;
  const char argument_separator = ',';
  ASTHandle *parsed_arguments = NULL;

  i = s.start;
  bool32 another_argument = true;
  while (i <= s.end)
    {
      if (t_arr[i].value == '[')
        {
          struct Statement list_statement = { i, s.end };
          ARRAY_PUSH (parsed_arguments,
                      parse_list (t_arr, list_statement, &i));
          another_argument = false;
        }
      else if (t_arr[i].value == argument_separator)
        {
          struct Statement statement;
          statement.start = start_statement;
          statement.end = i - 1;
          ARRAY_PUSH (parsed_arguments, parse_statement (t_arr, statement));

          /* skip the argument separator */
          start_statement = i + 1;
          another_argument = true;
        }
      ++i;
    }
  if (another_argument)
    {
      struct Statement statement;
      statement.start = start_statement;
      statement.end = i-1;
      ARRAY_PUSH (parsed_arguments, parse_statement (t_arr, statement));
    }
  return parsed_arguments;
}

ASTHandle *
parse_arguments_surrounded_by_parens (struct Token *t_arr,
                                      unsigned int start_index)
{
  /* if there are no args return null */
  if (t_arr[start_index].value == '(' &&
      t_arr[start_index + 1].value == ')')
    {
      return NULL;
    }
  /* if we have args inside parens */
  else if (t_arr[start_index].value == '(')
    {
      struct Statement s;
      unsigned int end_index = start_index + 1;
      size_t arr_length = ARRAY_COUNT (t_arr);
      s.start = start_index + 1;

      while (end_index < arr_length && t_arr[end_index].value != ')')
        {
          end_index++;
        }
      s.end = end_index - 1;
      return parse_arguments (t_arr, s);
    }
  else
    {
      printf ("Unable to parse arguments...\n");
      exit (1);
    }
}

ASTHandle
parse_possible_function(struct Token *t_arr, struct Statement s)
{
  ASTHandle node = 0;
  if (statement_size (s) > 1 && t_arr[s.start].value == TOK_FUNC)
    {
      unsigned int scope_index;
      struct Statement scope_statement;
      ASTHandle scope_handle;
      ASTHandle *func_args;
      struct AST *func_node;


      /* get function name */
      const char *func_name = t_arr[s.start + 1].string;
      /* parse function arguments */
      func_args = parse_arguments_surrounded_by_parens (t_arr, s.start + 2);
      /* parse function scope */
      scope_index = s.start;
      while (t_arr[scope_index].value != '{')
        scope_index++;
      scope_statement.start = scope_index;
      scope_statement.end = s.end;
      scope_handle = parse_scope (t_arr, scope_statement);

      /* fill out the func ast node */
      node = ast_get_node_handle ();
      func_node = ast_get_node (node);
      func_node->type = AST_FUNC;
      func_node->d.func_data.name = func_name;
      func_node->d.func_data.args = func_args;
      func_node->d.func_data.scope = scope_handle;
    }
  return node;
}

ASTHandle
parse_binary_op_bool_statement (struct Token *t_arr,
                                struct Statement s_indexes,
                                unsigned int op_index,
                                BinaryOpBoolType type)
{
  ASTHandle node = 0;
  struct Statement left_statement, right_statement;
  ASTHandle left_handle, right_handle;
  struct AST *equality_ast;

  /* split the statement into two, left expr and right */
  left_statement.start = s_indexes.start;
  left_statement.end = op_index - 1;

  switch (type)
    {
    case BOP_EQUAL:
    case BOP_NOT_EQUAL:
      right_statement.start = op_index + 2;
      break;
    case BOP_LESS_THAN:
    case BOP_GREATER_THAN:
      right_statement.start = op_index + 1;
      break;
    }
  right_statement.end = s_indexes.end;

  /* parse the left and right statements into ast nodes */
  left_handle = parse_statement (t_arr, left_statement);
  right_handle = parse_statement (t_arr, right_statement);

  /* create a new ast node for the equality statement */
  node = ast_get_node_handle ();
  equality_ast = ast_get_node (node);
  equality_ast->type = AST_BINARY_COMPARATOR;
  equality_ast->d.bcmp_data.left = left_handle;
  equality_ast->d.bcmp_data.right = right_handle;
  equality_ast->d.bcmp_data.type = type;

  /* don't need to keep moving through the for loop */
  return node;
}


ASTHandle
parse_possible_equality (struct Token *t_arr, struct Statement s_indexes)
{
  ASTHandle node = 0;
  /* FORMAT
   * Left_Expr == Right_Expr */
  unsigned int i;
  for (i = s_indexes.start; i < s_indexes.end; i++)
    {
      if (i < s_indexes.end - 1)
        {
          if (t_arr[i].value == '='  && t_arr[i+1].value == '=')
            return parse_binary_op_bool_statement (t_arr, s_indexes, i, BOP_EQUAL);
          else if (t_arr[i].value == '!' && t_arr[i+1].value == '=')
            return parse_binary_op_bool_statement (t_arr, s_indexes, i, BOP_NOT_EQUAL);
        }
      if (t_arr[i].value == '<')
        return parse_binary_op_bool_statement (t_arr, s_indexes, i, BOP_LESS_THAN);
      else if (t_arr[i].value == '>')
        return parse_binary_op_bool_statement (t_arr,
                                               s_indexes, 
                                               i,
                                               BOP_GREATER_THAN);


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
      ASTHandle scope_if_false = 0;
      struct Statement paren_statement;
      struct Statement scope_statement;
      unsigned int next_statement_index = s.end + 1;
      struct Statement next_statement;
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
      /* make sure we don't have an empty scope */
      if (statement_size (scope_statement) > 2)
        scope_if_true = parse_scope (t_arr, scope_statement);
	  
      /* if there subsequent if else statements, add them to the ast node */
      next_statement = get_statement (t_arr, &next_statement_index);
      if (t_arr[next_statement.start].value == TOK_ELSE) 
        {
	        if (next_statement.start < ARRAY_COUNT (t_arr))
            {
              if (t_arr[next_statement.start].value == TOK_ELSE)
                {
                  struct Statement else_statement;
                  /* we just want the scope part */
                  else_statement.start = next_statement.start + 1;
                  else_statement.end = next_statement.end;
                  scope_if_false = parse_scope (t_arr, else_statement);           

                  /* update the original statement index so the parser
                  * can skip the trailing parts of if */
                  global_statement_index = next_statement_index + 1;
                }
	          }
        }

    
      n = ast_get_node (node);
      n->d.if_data.scope_if_true = scope_if_true;
      n->d.if_data.scope_if_false = scope_if_false;
      n->d.if_data.expr = expr;
      n->type = AST_IF;
    }
  return node;
}
ASTHandle
parse_scope(struct Token* t_arr, struct Statement s)
{
  ASTHandle handle = 0; 
  struct AST *scope = NULL;
  unsigned int index = s.start + 1; /* skip '{' */
  unsigned int scope_number = 1;
  ASTHandle *statements = NULL;

  assert (t_arr[s.start].value == '{');

  if (s.start + 1 == s.end)
    return handle;

  handle = ast_get_node_handle ();
  while (scope_number != 0 )
    {
      ASTHandle statement;
      struct Statement statement_indexes = get_statement(t_arr, &index);

      /* this kind of sucks that we have to test for things we
       * should ignore, but complicating the get_statement logic
       * to accomodate for if and else statements being one statement
       * instead of two is not worth it */
      if (t_arr[statement_indexes.start].value != TOK_ELSE)
        {
          statement = parse_statement (t_arr, statement_indexes);
          ARRAY_PUSH (statements, statement);
        }

      if (t_arr[index].value == '}')
        scope_number--;
      if (t_arr[index].value == '{')
        scope_number++;

      if (scope_number <= 0)
        break;
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
      n->d.int_data.value = atof (t_arr[s.start].string);
    }
  return node;
}

bool32
is_arithmetic_op (TValue v)
{
  return (v == '+' || v == '-' || v == '/' || v == '*');
}

bool32
greater_precedence (const char left, const char right)
{
  if ('*' == left)
    {
      if (right == '+' || right == '-')
        return true;
    }
  else if ('/' == left)
    {
      if (right == '+' || right == '-')
        return true;
    }
  return false;
}
bool32
equal_precedence (const char left, const char right)
{
  if (('+' == left || '-' == left)
      && ('+' == right || '-' == right))
    {
      return true;
    }
  if (('*' == left || '/' == left)
      && ('*' == right || '/' == right))
    {
      return true;
    }
  return false;
}

QUEUE (ASTHandleQueue)
convert_infix_to_postfix (QUEUE (ASTHandleQueue) * expr_q)
{
  STACK (ASTHandleStack) s;
  QUEUE (ASTHandleQueue) q;
  STACK_STRUCT_INIT (TokenStack, &s, ASTHandle, 10);
  QUEUE_STRUCT_INIT (TokenQueue, &q, ASTHandle, 10);
  while (!QUEUE_EMPTY (expr_q))
    {
      ASTHandle n = QUEUE_FRONT (expr_q);
      QUEUE_POP (expr_q);
      struct AST *ast = ast_get_node (n);
      if (ast->type != AST_OP)
        {
          QUEUE_PUSH (&q, n);
        }
      else if (ast->type == AST_OP)
        {
          if (!STACK_EMPTY (&s))
            {
              ASTHandle top = STACK_FRONT (&s);
              struct AST *top_node = ast_get_node (top);

              const char left = top_node->d.op_data.op;
              const char right = ast->d.op_data.op;
              while ((greater_precedence (left, right))
                     || ((equal_precedence (left, right))
                         && (left != ')')))
                {
                  STACK_POP (&s);
                  QUEUE_PUSH (&q, top);

                  if (STACK_EMPTY (&s))
                    break;
                  top = STACK_FRONT (&s);
                }
            }
          STACK_PUSH (&s, n);
        }
      if (ast->type == AST_PAREN)
      {
        if (ast->d.paren_data.val == '(')
          {
            STACK_PUSH (&s, n);
          }
        else
          {
            ASTHandle tn = STACK_FRONT (&s);
            struct AST *tn_node = ast_get_node (tn);
            STACK_POP (&s);
            while (tn_node->type != AST_PAREN && tn_node->d.paren_data.val != '(')
              {
                QUEUE_PUSH (&q, tn);
                tn = STACK_FRONT (&s);
                tn_node = ast_get_node (tn);
                STACK_POP (&s);
              }
          }
      }
      /*
      if (n->value == STATEMENT_DELIM)
        break;
        */
    }
  while (!STACK_EMPTY (&s))
    {
      ASTHandle tn = STACK_FRONT (&s);
      STACK_POP (&s);
      QUEUE_PUSH (&q, tn);
    }
  STACK_FREE (&s, TokenStack);
  QUEUE_FREE (expr_q, TokenQueue);
  return q;
}
ASTHandle
convert_postfix_to_ast (QUEUE (ASTHandleQueue) * postfix_q)
{
  STACK (ASTHandleStack) s;
  ASTHandle return_handle;
  STACK_STRUCT_INIT (ASTHandleStack, &s, ASTHandle, 10);
  while (!QUEUE_EMPTY (postfix_q))
    {
      ASTHandle n = QUEUE_FRONT (postfix_q);
      struct AST *n_node = ast_get_node (n);
      QUEUE_POP (postfix_q);

      if (n_node->type != AST_OP)
        {
          STACK_PUSH (&s, n);
        }
      else if (n_node->type == AST_OP)
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
          n_node = ast_get_node (n); /* re-get n_node pointer */

          /* Fill out information of binary op */
          op = ast_get_node (ast_handle);
          op->type = AST_BINARY_OP;
          op->d.bop_data.left = left;
          op->d.bop_data.right = right;
          op->d.bop_data.op = n_node->d.op_data.op;
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
  QUEUE (ASTHandleQueue) expr_q = { 0 };
  QUEUE (ASTHandleQueue) postfix = { 0 };
  unsigned int i;
  ASTHandle op;

  QUEUE_STRUCT_INIT (ASTHandleQueue, &expr_q, ASTHandle, 10);

  unsigned int start_index = s.start;
  for (i = s.start; i < s.end; i++)
    {
      if (is_arithmetic_op (t_arr[i].value))
        {
          struct Statement sub_statement = { start_index, i-1 };
          ASTHandle h = parse_statement (t_arr, sub_statement);
          QUEUE_PUSH (&expr_q, h);

          struct Statement op_statement = { i, i };
          h = parse_statement (t_arr, op_statement);
          QUEUE_PUSH (&expr_q, h);
          
          start_index = i + 1;
        }
    }
  struct Statement sub_statement = { start_index, i };
  ASTHandle h = parse_statement (t_arr, sub_statement);
  QUEUE_PUSH (&expr_q, h);
  start_index = i + 1;

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
  if (statement_size (s) <= 1)
    return node;
  size_t i;
  for (i = s.start; i <= s.end ; i++)
    {
      if (is_arithmetic_op(t_arr[i].value))
        {
          node = parser__arithmetic (t_arr, s);
        }
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
  if ((statement_size (s) == 1 &&
       t_arr[s.start].value == TOK_STRING) ||
      (statement_size (s) >= 2 &&
      t_arr[s.start].value == TOK_STRING &&
      t_arr[s.start + 1].value == STATEMENT_DELIM))
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
