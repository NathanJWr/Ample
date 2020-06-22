#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#define MEM_DEBUG
#include "../mem_debug.c"
#include "../mem_debug.h"


#include "../ampobject.c"
#include "../ast.c"
#include "../dict_vars.c"
#include "../hash.c"
#include "../interpreter.c"
#include "../intobject.c"
#include "../lexer.c"
#include "../ncl.c"
#include "../parser.c"
#include "../ssl.c"
#include "../strobject.c"

#include "../test_helper.h"
bool test_if_statement_with_bool_true ()
{
  ASTHandle ast_head;
  AmpObject *obj = NULL;
  char *program = "if (true) {a = 10;}";
  struct Token *tokens = lex_all (program);
  
  ast_head = parse_tokens (tokens);
  interpreter_start (ast_head);
  obj = debug__interpreter_get_variable_object ("a");
  EXPECT (obj);
  EXPECT (obj->info->type == AMP_OBJ_INT);
  EXPECT (AMP_INTEGER (obj)->val == 10);

  return true;
}

bool test_if_statement_with_bool_false ()
{
  ASTHandle ast_head;
  AmpObject *obj = NULL;
  char *program = "if (false) {a = 10;}";
  struct Token *tokens = lex_all (program);
  
  ast_head = parse_tokens (tokens);
  interpreter_start (ast_head);
  obj = debug__interpreter_get_variable_object ("a");
  EXPECT (obj == NULL);
  return true;
}

int main ()
{
  TRY (test_if_statement_with_bool_true);
  TRY (test_if_statement_with_bool_false);
}
