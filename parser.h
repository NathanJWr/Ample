#ifndef PARSER_H_
#define PARSER_H_
enum ASTType {
    AST_INTEGER,
    AST_IDENTIFIER,
    AST_SCOPE,
};
/* declare here so sub-ast types can have pointers to the general struct */
struct AST;

struct ScopeAST {
    struct AST** statements; /* stb array */
};

struct IntegerAST {
    int value;
};
struct IdentifierAST {
    char* id; /* ssl string */
};
struct AST {
    enum ASTType type;
    union {
        struct ScopeAST scope_data;
        struct IntegerAST int_data;
        struct IdentifierAST id_data;
    };
};
#define STATEMENT_DELIM ';'
/* external functions */
struct Token;
struct AST parse_tokens (struct Token* tokens);


/* internal functions */
struct Statement parse__get_statement (struct Token* restrict tokens, unsigned int* restrict index);
struct AST* parse__statement (struct Token* t_arr, struct Statement s);

struct AST* parse__possible_integer (struct Token* t_arr, struct Statement s);
struct AST* parse__possible_identifier (struct Token* t_arr, struct Statement s);


static struct AST* ast_buffer;
struct AST* parse__allocate_ast ();




#endif // PARSER_H_