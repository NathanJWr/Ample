#ifndef NCL_H_
#define NCL_H_
#include <stdlib.h>
void* ncl_realloc (void* p, size_t s)
{
    void* tmp = realloc (p, s);
    if (tmp == NULL) {
        return NULL;
    } else {
        return tmp;
    }
}
#endif // NCL_H_