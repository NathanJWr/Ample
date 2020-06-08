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
