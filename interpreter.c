#include "interpreter.h"
#include "array.h"
void interpreter_start (ASTHandle head)
{
    struct AST* h = ast_get_node (head);
    if (h->type == AST_SCOPE) {
        for (unsigned int i = 0; i < ARRAY_COUNT (h->scope_data.statements); i++) {
            interpreter__evaluate_statement (h->scope_data.statements[i]);
        }
    }
}

void interpreter__evaluate_statement (ASTHandle statement)
{
    struct AST* s = ast_get_node (statement);
    if (s->type == AST_ASSIGNMENT) {
        interpreter__evaluate_assignment (statement);
    } else if (s->type == AST_BINARY_OP) {
        interpreter__evaluate_binary_op (statement);
    }
}

int interpreter__evaluate_binary_op (ASTHandle handle)
{
    int left_value, right_value;
    struct AST* node = ast_get_node (handle);

    if (node->type == AST_BINARY_OP) {
        ASTHandle right_handle = node->bop_data.right;
        ASTHandle left_handle = node->bop_data.left;
        
        struct AST* right = ast_get_node (right_handle);
        if (right->type == AST_INTEGER) {
            right_value = right->int_data.value;
        } else if (right->type == AST_BINARY_OP) {
            right_value = interpreter__evaluate_binary_op (right_handle);
        }

        struct AST* left = ast_get_node (left_handle);
        if (left->type == AST_INTEGER) {
            left_value = left->int_data.value;
        } else if (left->type == AST_BINARY_OP) {
            left_value = interpreter__evaluate_binary_op (left_handle);
        }

        switch (node->bop_data.op) {
            case '+': return right_value + left_value; break;
            case '-': return right_value - left_value; break;
            case '*': return right_value * left_value; break;
            case '/': return right_value / left_value; break;
            default: return 0;
        }
    }
    return 0;
}

void interpreter__evaluate_assignment (statement)
{
    struct AST* s = ast_get_node (statement);
    struct AST* expr = ast_get_node (s->asgn_data.expr);
    if (expr->type == AST_INTEGER) {
        
    }
}
