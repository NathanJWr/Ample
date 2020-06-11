#include "../hash.c"
#include "../ncl.c"
#include "../test_helper.h"
DICT_DECLARE (IntVars, const char*, int);
DICT_IMPL (IntVars, const char*, int); 

bool
test_hash_erase ()
{
  DICT (IntVars) d = { 0 };
  DictIntVars_init (&d, hash_string, string_compare, 1);
  char *var = "abc";
  char *var2 = "123";
  DictIntVars_insert (&d, var, 10);
  DictIntVars_insert (&d, var2, 20);
  /* erase one variable */
  DictIntVars_erase (&d, var);
  EXPECT (d.count == 1);
  int val;
  bool ret = DictIntVars_get (&d, var2, &val);
  EXPECT (ret == true);
  EXPECT (val == 20);
  ret = DictIntVars_get(&d, var, &val);
  EXPECT (ret == false);

  DictIntVars_free (&d);
  return true;
}
int
main ()
{
  TRY (test_hash_erase);
}
