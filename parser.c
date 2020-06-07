#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "stretchy_buffer.h"
#include "queue.h"
unsigned int statement_size (struct Statement s)
{
    return s.end - s.start + 1;
}

ASTHandle parse_tokens (struct Token* tokens)
{
    unsigned int index = 0;
    unsigned int head = ast_get_node_handle ();
    unsigned int* statements = NULL;
    while (index < sb_count (tokens)) {
        struct Statement s = parse__get_statement(tokens, &index);
        sb_push (statements, parse__statement (tokens, s));
    }

    struct AST* h = ast_get_node (head);
    *h = (struct AST) {
        .type = AST_SCOPE,
        .scope_data = {
            .statements = statements,
        }
    };
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

ASTHandle parse__statement (struct Token* t_arr, struct Statement s)
{
    ASTHandle node = 0;
    node = parser__possible_assignment (t_arr, s);
    if (node) return node;

    node = parser__possible_integer (t_arr, s);
    if (node) return node;

    node = parser__possible_identifier (t_arr, s);
    if (node) return node;

    node = parser__possible_arithmetic (t_arr, s);
    if (node) return node;

    node = parser__possible_string (t_arr, s);
    if (node) return node;

    return 0;
}
ASTHandle parser__possible_integer (struct Token* t_arr, struct Statement s)
{
    ASTHandle node = 0;
    if (statement_size (s)  == 1 &&
        t_arr[s.start].value == TOK_INTEGER) { /* INTEGER literal */
        node = ast_get_node_handle ();
        struct AST* n = ast_get_node (node);
        *n = (struct AST) {
            .type = AST_INTEGER,
            .int_data = {
                .value = atoi (t_arr[s.start].string),
            },
        };
    }
    return node;
}

bool parser__is_arithmetic_op (TValue v)
{
    return (v == '+' || v == '-' || v == '/' || v == '*');
}

bool parser__greater_precedence (struct Token* left, struct Token* right)
{
    if ('*' == left->value) {
        if (right->value == '+' || right->value == '-')
            return true;
    } else if ('/' == left->value) {
        if (right->value == '+' || right->value == '-')
            return true;
    }
    return false;
    
}
bool parser__equal_precedence (struct Token* left, struct Token* right)
{
    if (('+' == left->value || '-' == left->value) &&
        ('+' == right->value || '-' == right->value)) {
        return true;
    }
    if (('*' == left->value || '/' == left->value) &&
        ('*' == right->value || '/' == right->value)) {
        return true;
    }
    return false;
}
struct TokenTailQ parser__convert_infix_to_postfix (struct TokenTailQ* expr_q)
{
    struct TokenTailQ q = STAILQ_HEAD_INITIALIZER (q);
    struct TokenStack s = SLIST_HEAD_INITIALIZER (s);
    SLIST_INIT (&s);
    STAILQ_INIT (&q);

    while (!STAILQ_EMPTY(expr_q)) {
        struct TokenNode* n = STAILQ_FIRST(expr_q);
        STAILQ_REMOVE_HEAD (expr_q, tqentries);

        if (n->tok->value == TOK_INTEGER || n->tok->value == TOK_IDENTIFIER) {
            STAILQ_INSERT_TAIL (&q, n, tqentries);
        }
        if (parser__is_arithmetic_op (n->tok->value)) {
            if (!SLIST_EMPTY (&s)) {
                struct TokenNode* top_token = SLIST_FIRST (&s);
                while ((parser__greater_precedence(top_token->tok, n->tok)) ||
                       ((parser__equal_precedence(top_token->tok, n->tok)) && (top_token->tok->value != ')'))) {
                    SLIST_REMOVE_HEAD (&s, slentries); /* pop from the stack */
                    STAILQ_INSERT_TAIL (&q, top_token, tqentries); /* push the stack value on to the queue */

                    if (SLIST_EMPTY (&s)) break;
                    top_token = SLIST_FIRST (&s);
                }
            }
            SLIST_INSERT_HEAD (&s, n, slentries); /* push the token on to the stack of operators */
        }
    
        if (n->tok->value == '(') {
            SLIST_INSERT_HEAD (&s, n, slentries);
        }
        if (n->tok->value == ')') {
            struct TokenNode* tn = SLIST_FIRST (&s);
            SLIST_REMOVE_HEAD (&s, slentries);
            while (tn->tok->value != '(') {
                STAILQ_INSERT_TAIL (&q, tn, tqentries);
                tn = SLIST_FIRST (&s);
                SLIST_REMOVE_HEAD (&s, slentries);
            }
        }
        if (n->tok->value == STATEMENT_DELIM) break;

    }
    /* Empty the stack into the queue */
    struct TokenNode* tn;
    while (!SLIST_EMPTY (&s)) {
        tn = SLIST_FIRST (&s);
        SLIST_REMOVE_HEAD (&s, slentries);
        STAILQ_INSERT_TAIL (&q, tn, tqentries);
    }
    return q;
}
ASTHandle parser__convert_postfix_to_ast (struct TokenTailQ postfix_q, unsigned int expr_size)
{
    struct ASTHandleStack s = SLIST_HEAD_INITIALIZER (s);
    SLIST_INIT (&s);
    struct ASTNode nodes[expr_size];
    unsigned int index = 0;
    while (!STAILQ_EMPTY (&postfix_q)) {
        struct TokenNode* n = STAILQ_FIRST (&postfix_q);
        STAILQ_REMOVE_HEAD (&postfix_q, tqentries);

        if (n->tok->value == TOK_INTEGER) {
            ASTHandle ast_handle = ast_get_node_handle ();
            struct AST* integer_ast = ast_get_node (ast_handle);
            integer_ast->type = AST_INTEGER;
            integer_ast->int_data.value = atoi (n->tok->string);

            /* insert the integer ast on to the stack */
            nodes[index].ast = ast_handle;
            SLIST_INSERT_HEAD (&s, &nodes[index], slentries);
            index++;
        } else if (parser__is_arithmetic_op (n->tok->value)) {
            struct ASTNode *left, *right;
            ASTHandle ast_handle;
            struct AST* op;

            /* Remove the first two elements from the stack.
               They will be the left and right params of a binary op */
            left = SLIST_FIRST (&s);
            SLIST_REMOVE_HEAD (&s, slentries);
            right = SLIST_FIRST (&s);
            SLIST_REMOVE_HEAD (&s, slentries);
            ast_handle = ast_get_node_handle ();
    
            /* Fill out information of binary op */
            op = ast_get_node (ast_handle);
            *op = (struct AST) {
                .type = AST_BINARY_OP,
                .bop_data = {
                    .left = left->ast,
                    .right = right->ast,
                    .op = n->tok->value
                }
            };
            
            /* Insert the operation, instead of the left and right values
               back on to the stack */
            nodes[index].ast = ast_handle;
            SLIST_INSERT_HEAD (&s, &nodes[index], slentries);
            index++;
        }
    }

    struct ASTNode* n;
    SLIST_FOREACH (n, &s, slentries) {
        printf ("%d ", n->ast);
    }

    n = SLIST_FIRST (&s);
    return n->ast;
}

ASTHandle parser__arithmetic (struct Token* t_arr, struct Statement s)
{
    /* storage necessary to put tokens into a queue/stack 
       no more memory should be allocated for this process */
    struct TokenTailQ expr_q = STAILQ_HEAD_INITIALIZER (expr_q);
    struct TokenNode nodes[statement_size (s) * sizeof(struct TokenNode)];
    STAILQ_INIT (&expr_q);
    for (unsigned int i = s.start; i <= s.end; i++) {
        nodes[i].tok = t_arr + i;
        STAILQ_INSERT_TAIL (&expr_q, &nodes[i], tqentries);
    }
    struct TokenTailQ postfix = parser__convert_infix_to_postfix (&expr_q);
    parser__debug_print_queue (&postfix);
    ASTHandle op = parser__convert_postfix_to_ast (postfix, statement_size (s));
    return op;
}


void parser__debug_print_queue (struct TokenTailQ* q)
{
    struct TokenNode* np;
    STAILQ_FOREACH (np, q, tqentries) {
        if (np->tok->value < 128)
            printf ("Value: %c ", np->tok->value);
        else 
            printf ("Value: %d ", np->tok->value);
        if (np->tok->string)
            printf("String: %s", np->tok->string);
        printf("\n");
    }
}

ASTHandle parser__possible_arithmetic (struct Token* t_arr, struct Statement s)
{
    ASTHandle node = 0;
    if (statement_size (s) >= 2 &&
        t_arr[s.start].value == TOK_INTEGER &&
        parser__is_arithmetic_op (t_arr[s.start + 1].value)) {
        node = parser__arithmetic (t_arr, s);
    }
    return node;
}

ASTHandle parser__possible_identifier (struct Token* t_arr, struct Statement s)
{
    ASTHandle node = 0;
    if (statement_size (s) == 1 &&
        t_arr[s.start].value == TOK_IDENTIFIER) {
        node = ast_get_node_handle ();
        struct AST* n = ast_get_node (node);
        *n = (struct AST) {
            .type = AST_IDENTIFIER,
            .id_data = {
                .id = t_arr[s.start].string,
            }
        };
    }
    return node;
}

ASTHandle parser__possible_string (struct Token* t_arr, struct Statement s)
{
    ASTHandle node = 0;
    if (statement_size (s) == 1 &&
        t_arr[s.start].value == TOK_STRING) {
        node = ast_get_node_handle ();
        struct AST* n = ast_get_node (node);
        *n = (struct AST) {
            .type = AST_STRING,
            .str_data = {
                .str = t_arr[s.start].string,
            }
        };
    }
    return node;
}

ASTHandle parser__possible_assignment (struct Token* t_arr, struct Statement s)
{
    ASTHandle node = 0;
    /* VAR = EXPR */
    if (statement_size (s) >= 3 &&
        t_arr[s.start].value == TOK_IDENTIFIER &&
        t_arr[s.start+1].value == '=' &&
        t_arr[s.start+2].value != '=') {
        node = ast_get_node_handle ();

        struct Statement sub_statement = {
            .start = s.start + 2,
            .end = s.end,
        };
        struct AST ast =  {
            .type = AST_ASSIGNMENT,
            .asgn_data = {
                .var = t_arr[s.start].string,
                .expr = parse__statement (t_arr, sub_statement),
            }
        };
        struct AST* n = ast_get_node (node);
        *n = ast;
    }
    return node;
}