#ifndef STRING_H_
#define STRING_H_

#include <mem.h>
#include <list.h>

char* strcat(char* destination, const char* source);
char* strcpy(char* destination, const char* source);
size_t strlen(const char* str);
char* strdup(const char* src);
int strcmp(const char *x, const char *y);
char* strstr(const char *x, const char *y);

char* list2str(list_t* list, const char* delim);
char* strsep(char** stringp, const char* delim);
list_t* str_split(const char* str, const char* delim, unsigned int* numtokens);

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

char* list2str(list_t* list, const char* delim) {
    char * ret = malloc(256);
    memset(ret, 0, 256);
    int len = 0, ret_len = 256;
    while(list_size(list)> 0) {
        char * temp = list_pop(list)->val;
        int len_temp = strlen(temp);
        if(len + len_temp + 1 + 1 > ret_len) {
            ret_len = ret_len * 2;
            ret = realloc(ret, ret_len);
            len = len + len_temp + 1;
        }
        strcat(ret, delim);
        strcat(ret, temp);
    }
    return ret;
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

list_t* str_split(const char* str, const char* delim, unsigned int* numtokens) {
    list_t* ret_list = list_create();
    char* s = strdup(str);
    char* token, *rest = s;
    while ((token = strsep(&rest, delim)) != NULL) {
        if(!strcmp(token, ".")) continue;
        if(!strcmp(token, "..")) {
            if(list_size(ret_list) > 0) list_pop(ret_list);
            continue;
        }
        list_push(ret_list, strdup(token));
        if(numtokens) (*numtokens)++;
    }
    free(s);
    return ret_list;
}

#endif