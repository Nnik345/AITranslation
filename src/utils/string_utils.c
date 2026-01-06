#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "string_utils.h"

char *StrDup(const char *s) {
    if (!s) return NULL;
    char *d = malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

char *StrTrim(char *s) {
    if (!s) return NULL;
    
    // Trim leading
    while(isspace((unsigned char)*s)) s++;
    
    if(*s == 0) return s; // All spaces
    
    // Trim trailing
    char *end = s + strlen(s) - 1;
    while(end > s && isspace((unsigned char)*end)) end--;
    
    *(end+1) = '\0';
    return s;
}

void CsvFreeTokens(char **tokens, int count) {
    if (!tokens) return;
    for (int i = 0; i < count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

/* 
 * Robust CSV Line Tokenizer 
 * Handles "quoted, fields" and escaped ""quotes"".
 */
char **CsvTokenizeLine(const char *line, int *count) {
    if (!line) { *count=0; return NULL; }

    int capacity = 10;
    int nTokens = 0;
    char **tokens = malloc(sizeof(char*) * capacity);
    if(!tokens) return NULL;

    const char *p = line;
    
    while (*p) {
        if (nTokens >= capacity) {
            capacity *= 2;
            char **newTokens = realloc(tokens, sizeof(char*) * capacity);
            if (!newTokens) { CsvFreeTokens(tokens, nTokens); *count=0; return NULL; }
            tokens = newTokens;
        }

        // Parse one field
        int bufCap = 64;
        int bufLen = 0;
        char *field = malloc(bufCap);
        
        int inQuote = 0;
        // Check if field starts with quote
        if (*p == '"') {
            inQuote = 1;
            p++;
        }

        while (*p) {
            // Resize buffer if needed
            if (bufLen + 1 >= bufCap) {
                bufCap *= 2;
                char *newField = realloc(field, bufCap);
                if (!newField) { free(field); CsvFreeTokens(tokens, nTokens); *count=0; return NULL; }
                field = newField;
            }

            if (inQuote) {
                if (*p == '"') {
                    if (*(p+1) == '"') {
                        // Escaped quote: "" -> "
                        field[bufLen++] = '"';
                        p += 2;
                    } else {
                        // End of quote
                        inQuote = 0;
                        p++;
                    }
                } else {
                    field[bufLen++] = *p++;
                }
            } else {
                if (*p == ',') {
                    p++; // consume comma
                    break; // End of field
                } else if (*p == '\r' || *p == '\n') {
                    // End of line found. Consume newline chars so outer loop terminates (points to \0)
                    while (*p == '\r' || *p == '\n') p++;
                    break; 
                } else {
                    field[bufLen++] = *p++;
                }
            }
        }
        field[bufLen] = '\0';
        tokens[nTokens++] = field;
        
        // Check for trailing comma which implies an empty field at end of line.
        // Current logic might miss it if logic relies on while(*p).
        // For robustness, one could check if the last consumed char was a comma.
        // However, standard compliant CSVs often end with CRLF.
    }
    
    *count = nTokens;
    return tokens;
}
