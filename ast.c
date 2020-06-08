#include "ast.h"
#include "array.h"
static struct AST* ast_buffer;
unsigned int ast_get_node_handle ()
{
    struct AST a = {0};
    ARRAY_PUSH (ast_buffer, a);
    return ARRAY_COUNT(ast_buffer) - 1;
}
struct AST* ast_get_node (unsigned int index)
{
    return &ast_buffer[index];
}
void ast_free_buffer ()
{
    for (unsigned int i = 0; i < ARRAY_COUNT (ast_buffer); i++) {
        if (ast_buffer[i].type == AST_SCOPE) {
            ARRAY_FREE (ast_buffer[i].scope_data.statements);
        }
    }
    ARRAY_FREE (ast_buffer);
}