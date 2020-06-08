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
#ifndef AST_H_
#define AST_H_
#include "lexer.h"
typedef unsigned int ASTHandle;
enum ASTType {
    AST_INTEGER,
    AST_STRING,
    AST_IDENTIFIER,
    AST_SCOPE,
    AST_BINARY_OP,
    AST_ASSIGNMENT,
};
/* declare here so sub-ast types can have pointers to the general struct */
struct AST;

struct ScopeAST {
    unsigned int* statements; /* sb array */
};

struct IntegerAST {
    int value;
};

struct StringAST {
    char* str;
};

struct IdentifierAST {
    char* id; /* ssl string */
};
struct BinaryOpAST {
    ASTHandle left;
    ASTHandle right;
    TValue op;
};
struct AssignmentAST {
    char* var;
    ASTHandle expr;
};
struct AST {
    enum ASTType type;
    union {
        struct ScopeAST scope_data;
        struct IntegerAST int_data;
        struct IdentifierAST id_data;
        struct BinaryOpAST bop_data; 
        struct StringAST str_data;
        struct AssignmentAST asgn_data;
    };
};


unsigned int ast_get_node_handle ();
struct AST* ast_get_node (unsigned int index);

/* call this AFTER the ast is done being used */
void ast_free_buffer ();
#endif // AST_H_