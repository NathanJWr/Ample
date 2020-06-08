#include "ncl.h"
void* ncl_realloc (void* p, size_t s)
{
    void* tmp = realloc (p, s);
    if (tmp == NULL) {
        return NULL;
    } else {
        return tmp;
    }
}