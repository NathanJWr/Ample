#include "../array.h"

int main()
{
    int* arr = NULL;
    int i;
    ARRAY_PUSH (arr, 1);
    for (i = 0; i < 10000; i++) {
        ARRAY_PUSH (arr, i);
    }
    ARRAY_FREE (arr);
}
