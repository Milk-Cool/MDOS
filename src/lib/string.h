#ifndef STRING_H_
#define STRING_H_

#include <mem.h>

char* strcat(char* destination, const char* source);
char* strcpy(char* destination, const char* source);
size_t strlen(const char* str);
char* strdup(const char* src);
int strcmp(const char *x, const char *y);
char* strstr(const char *x, const char *y);
char* strsep(char** stringp, const char* delim);
char* strcat(char* destination, const char* source) {
    strcpy(destination + strlen(destination), source);
    return destination;
}
char* strcpy(char* destination, const char* source) {
    if (destination == NULL) {
        return NULL;
    }

    char *ptr = destination;

    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }

    *destination = '\0';

    return ptr;
}
size_t strlen(const char* str) {
    size_t o = 0;
    while(str[o] != '\0')
        o++;
    return o;
}

char* strdup(const char* src) {
    size_t len = strlen(src);
    char* o = malloc(len + 1);
    strcpy(o, src);
    o[len] = '\0';
    return o;
}

int strcmp(const char* x, const char* y) {
    while (*x)
    {
        if (*x != *y) {
            break;
        }

        x++;
        y++;
    }

    return *(const unsigned char*)x - *(const unsigned char*)y;
}

char* strstr(const char* x, const char* y) {
    size_t n = strlen(y);
 
    while (*x) {
        if (!memcmp(x, y, n)) {
            return (char*) x;
        }
 
        x++;
    }
 
    return 0;
}

char* strsep(char** stringp, const char* delim) {
    char* s;
    const char* spanp;
    int c, sc;
    char* tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
}

#endif