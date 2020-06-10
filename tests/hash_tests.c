#include "../test_helper.h"
#include "../hash.c"
#include "../ncl.c"
DICT_DECLARATION(IntVars, const char*, int);
bool test_hash_erase ()
{
  DICT(IntVars) d = {0};
  DICT_INIT(&d, hash_string, string_compare, 1);
  char* var = "abc";
  char* var2 = "123";
  DICT_INSERT(IntVars, &d, var, 10);
  DICT_INSERT(IntVars, &d, var2, 20);

  /* erase one variable */
  DICT_ERASE(IntVars, &d, var);
  EXPECT (DICT_COUNT (&d) == 1);
  int val;
  bool ret = DICT_AT (IntVars, &d, var2, &val);
  EXPECT (ret == true);
  EXPECT (val == 20);
  ret = DICT_AT (IntVars, &d, var, &val);
  EXPECT (ret == false);

  DICT_FREE (&d);
  return true;
}
int main ()
{
  TRY (test_hash_erase);
}
