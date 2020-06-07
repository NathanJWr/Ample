#include "../myqueue.h"
#include "../test_helper.h"
#include <stdbool.h>
QUEUE_DECLARATION (Integer, int);

bool test_queue_push_pop_all ()
{
    QUEUE (Integer) q = QUEUE_STRUCT_INIT (Integer, int, 50);
    for (int i = 0; i < 800; i++) {
        QUEUE_PUSH (&q, i);
    }
    for (int i = 0; i < 800; i++) {
        int var = QUEUE_FRONT (&q);
        EXPECT (var == i);
        QUEUE_POP (&q);
    }
    QUEUE_FREE (&q, Integer);
    return true;
}

bool test_queue_push_pop_half ()
{
    QUEUE (Integer) q = QUEUE_STRUCT_INIT (Integer, int, 1);
    for (int i = 0; i < 10; i++) {
        QUEUE_PUSH (&q, i);
    }
    for (int i = 0; i < 5; i++) {
        QUEUE_POP (&q);
    }
    for (int i = 100; i < 110; i++) {
        QUEUE_PUSH (&q, i);
    }

    for (int i = 5; i < 10; i++) {
        int var = QUEUE_FRONT (&q);
        QUEUE_POP (&q);

        EXPECT (var == i);
    }
    for (int i = 100; i < 110; i++) {
        int var = QUEUE_FRONT (&q);
        QUEUE_POP (&q);

        EXPECT (var == i);
    }
    QUEUE_FREE (&q, Integer);
    return true;
}

QUEUE_DECLARATION (IntegerPointer, int*);
bool test_queue_with_pointers ()
{
    int nums[100];
    QUEUE(IntegerPointer) q = QUEUE_STRUCT_INIT (IntegerPointer, int*, 100);
    for (int i = 0; i < 100; i++) {
        nums[i] = i+10;
        QUEUE_PUSH (&q, nums+i);
    }

    for (int i = 0; i < 100; i++) {
        int* num = QUEUE_FRONT (&q);
        QUEUE_POP (&q);
        EXPECT (*num == i + 10);
    }
    QUEUE_FREE (&q, IntegerPointer);
    return true;
}

int main () {
    TRY (test_queue_push_pop_all);
    TRY (test_queue_push_pop_half);
    TRY (test_queue_with_pointers);
}