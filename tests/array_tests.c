#include "../array.h"

int main()
{
    int* arr = NULL;
    ARRAY_PUSH (arr, 1);
    ARRAY_ADD (arr, 10);
    for (int i = 0; i < 10000; i++) {
        ARRAY_PUSH (arr, i);
    }
    ARRAY_FREE (arr);
}