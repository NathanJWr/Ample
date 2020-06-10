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
unsigned int
ast_get_node_handle ()
{
  struct AST a = { 0 };
  ARRAY_PUSH (ast_buffer, a);
  return ARRAY_COUNT (ast_buffer) - 1;
}
struct AST *
ast_get_node (unsigned int index)
{
  return &ast_buffer[index];
}
void
ast_free_buffer ()
{
  for (unsigned int i = 0; i < ARRAY_COUNT (ast_buffer); i++)
    {
      if (ast_buffer[i].type == AST_SCOPE)
        {
          ARRAY_FREE (ast_buffer[i].scope_data.statements);
        }
    }
  ARRAY_FREE (ast_buffer);
}
