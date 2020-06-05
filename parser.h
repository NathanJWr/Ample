#ifndef PARSER_H_
#define PARSER_H_
#define STATEMENT_DELIM ';'
/* external functions */
struct Token;
struct AST* parse_tokens (struct Token* tokens);


/* internal functions */
struct Statement parse__get_statement (struct Token* restrict tokens, unsigned int* restrict index);
unsigned int parse__statement (struct Token* t_arr, struct Statement s);

unsigned int parse__possible_integer (struct Token* t_arr, struct Statement s);
unsigned int parse__possible_identifier (struct Token* t_arr, struct Statement s);
#endif // PARSER_H_