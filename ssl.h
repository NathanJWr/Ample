#ifndef SSL_H_
#define SSL_H_
#include <string.h>
#include <bsd/string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
struct _SSLString {
    uint32_t size;
    uint32_t length;
    char string[];
};

// Access elements

#define SSL_BASE_POINTER(p_str) ((struct _SSLString*) (p_str - sizeof(struct _SSLString)))
#define DEFAULT_RESIZE(size) size*2
void ssl_free (char* str);
char* ssl_resize (char* str, uint32_t size);
char* ssl_strcpy (char* dest, char* str);
char* ssl_strcat (char* dest, char* src);
char* ssl_addchar (char* str, char c);


#endif // SSL_H_