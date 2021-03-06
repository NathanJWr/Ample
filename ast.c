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

#include "array.h"
#include "ast.h"
static struct AST *ast_buffer;
size_t
ast_get_node_handle ()
{
  struct AST a = { 0 };
  ARRAY_PUSH (ast_buffer, a);
  return ARRAY_COUNT (ast_buffer);
}
struct AST *
ast_get_node (ASTHandle index)
{
  if (index == 0)
    return NULL;
  else
    return &ast_buffer[index-1];
}
void
ast_free_buffer ()
{
  size_t i = 0;
  size_t count = ARRAY_COUNT (ast_buffer);
  for (i = 0; i < count; i++)
    {
      if (ast_buffer[i].type == AST_SCOPE)
        {
          ARRAY_FREE (ast_buffer[i].d.scope_data.statements);
        }
      if (ast_buffer[i].type == AST_FUNC)
        {
          ARRAY_FREE (ast_buffer[i].d.func_data.args);
        }
      if (ast_buffer[i].type == AST_FUNC_CALL)
        {
          ARRAY_FREE (ast_buffer[i].d.func_call_data.args);
        }
      if (ast_buffer[i].type == AST_LIST)
        {
          ARRAY_FREE (ast_buffer[i].d.list_data.items);
        }
    }
  ARRAY_FREE (ast_buffer);
}
