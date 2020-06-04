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
    unsigned int* statements;
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
struct AST* parse_tokens (struct Token* tokens);
/* call this AFTER the ast is done being used */
void free_ast_buffer ();

/* internal functions */
struct Statement parse__get_statement (struct Token* restrict tokens, unsigned int* restrict index);
unsigned int parse__statement (struct Token* t_arr, struct Statement s);

unsigned int parse__possible_integer (struct Token* t_arr, struct Statement s);
unsigned int parse__possible_identifier (struct Token* t_arr, struct Statement s);


static struct AST* ast_buffer;

/* returns an integer that can be used to access the ast in memory */
unsigned int parse__allocate_ast ();
struct AST* parse__get_ast(unsigned int index);




#endif // PARSER_H_