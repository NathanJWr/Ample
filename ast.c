#include "ast.h"
#include "stretchy_buffer.h"
static struct AST* ast_buffer;
unsigned int ast_get_node_handle ()
{
    struct AST a = {0};
    sb_push (ast_buffer, a);
    return sb_count(ast_buffer) - 1;
}
struct AST* ast_get_node (unsigned int index)
{
    return &ast_buffer[index];
}
void ast_free_buffer ()
{
    for (unsigned int i = 0; i < sb_count (ast_buffer); i++) {
        if (ast_buffer[i].type == AST_SCOPE) {
            sb_free (ast_buffer[i].scope_data.statements);
        }
    }
    sb_free (ast_buffer);
}