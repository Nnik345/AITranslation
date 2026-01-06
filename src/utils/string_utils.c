#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "string_utils.h"

char *StrDup(const char *s) {
    if (!s) return NULL;
    
    size_t byteLen = strlen(s);
    char *copy = malloc(byteLen + 1);
    if (!copy) return NULL;
    
    memcpy(copy, s, byteLen);
    copy[byteLen] = '\0';
    return copy;
}

char *StrTrim(char *s) {
    if (!s) return NULL;
    
    return s;
}
