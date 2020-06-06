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
};
/* declare here so sub-ast types can have pointers to the general struct */
struct AST;

struct ScopeAST {
    unsigned int* statements;
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
struct AST {
    enum ASTType type;
    union {
        struct ScopeAST scope_data;
        struct IntegerAST int_data;
        struct IdentifierAST id_data;
        struct BinaryOpAST bop_data; 
        struct StringAST str_data;
    };
};


unsigned int ast_get_node_handle ();
struct AST* ast_get_node (unsigned int index);

/* call this AFTER the ast is done being used */
void ast_free_buffer ();
#endif // AST_H_