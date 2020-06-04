#ifndef TEST_HELPER_H_
#define TEST_HELPER_H_
#include <stdio.h>

#define TRY(func) do { \
  bool ret = func(); \
  if (ret) { \
    printf("\033[0;32m[PASSED] %s\n", #func); \
  } \
} while(0)

#define EXPECT_EQ(val1, val2) \
  if (val1 != val2) \
    printf("\033[0;31m %s failed", __func__);

#define EXPECT(val) \
  if (!(val)) { \
    printf("\033[0;31m[FAILED] %s, expected \"%s\" to be true\n", __func__, #val); \
    return false; \
  }
#endif //
