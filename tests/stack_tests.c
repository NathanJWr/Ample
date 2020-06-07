#include "../stack.h"
#include "../test_helper.h"
#include <stdbool.h>

STACK_DECLARATION (Integer, int);
bool test_stack_push_pop_all ()
{
    STACK(Integer) s = STACK_STRUCT_INIT (Integer, int, 10);
    for (int i = 0; i < 50; i++) {
        STACK_PUSH (&s, i);
    }
    for (int i = 49; i >= 0; i--) {
        int var = STACK_FRONT (&s);
        EXPECT (i == var);
        STACK_POP(&s);
    }
    STACK_FREE (&s, Integer);
    return true;
}
int main ()
{
    TRY (test_stack_push_pop_all);
}