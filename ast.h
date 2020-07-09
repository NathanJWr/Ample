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
#include "bool.h"
#include "dict_vars.h"
typedef size_t ASTHandle;
enum ASTType {
  AST_INTEGER,
  AST_STRING,
  AST_IDENTIFIER,
  AST_SCOPE,
  AST_BINARY_OP,
  AST_ASSIGNMENT,
  AST_IF,
  AST_BOOL,
  AST_EQUALITY,
  AST_FUNC,
  AST_FUNC_CALL,
  AST_OP,
  AST_PAREN,
};
/* declare here so sub-ast types can have pointers to the general struct */
struct AST;

struct ScopeAST {
  ASTHandle *statements; /* sb array */
  DICT(ObjVars) *variable_scope_stack;
};

struct IntegerAST {
  double value;
};

struct StringAST {
  char *str;
};

struct IdentifierAST {
  char *id; /* ssl string */
};
struct BinaryOpAST {
  ASTHandle left;
  ASTHandle right;
  TValue op;
};
struct AssignmentAST {
  char *var;
  ASTHandle expr;
};
struct IfAST {
  ASTHandle expr;
  ASTHandle scope_if_true;
  ASTHandle scope_if_false;
};
struct BoolAST {
  bool32 value;
};

typedef enum {
  BOP_EQUAL,
  BOP_NOT_EQUAL,
  BOP_LESS_THAN,
  BOP_GREATER_THAN,
} BinaryOpBoolType;

struct EqualityAST {
  ASTHandle left;
  ASTHandle right;
  BinaryOpBoolType type;
};
struct FuncAST {
  const char *name;
  ASTHandle *args;
  ASTHandle scope;
};
struct FuncCallAST {
  const char *name;
  ASTHandle *args;
};
struct OpAST {
  char op;
};
struct ParenAST {
  char val;
};
struct AST {
  enum ASTType type;
  union data {
    struct ScopeAST scope_data;
    struct IntegerAST int_data;
    struct IdentifierAST id_data;
    struct BinaryOpAST bop_data;
    struct StringAST str_data;
    struct AssignmentAST asgn_data;
    struct IfAST if_data;
    struct BoolAST bool_data;
    struct EqualityAST equality_data;
    struct FuncAST func_data;
    struct FuncCallAST func_call_data;
    struct OpAST op_data;
    struct ParenAST paren_data;
  } d;
};

size_t ast_get_node_handle();
struct AST *ast_get_node(ASTHandle index);

/* call this AFTER the ast is done being used */
void ast_free_buffer();
#endif
