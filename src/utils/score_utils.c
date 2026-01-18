/**
 * @file score_utils.c
 * @brief Specialized tokenization for machine translation evaluation metrics.
 * 
 * Implements a UTF-8 aware tokenizer that separates punctuation for fair
 * metric calculation (especially for BLEU).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scoring.h"
#include "string_utils.h"

/**
 * @brief Tokenizes input text into a list of words and punctuation marks.
 * 
 * Tokenization rules:
 * 1. Sequences of ASCII whitespace are ignored.
 * 2. Every ASCII punctuation character is treated as a separate token.
 * 3. Multi-byte UTF-8 sequences (Indian scripts) are treated as part of words.
 * 
 * @param text UTF-8 encoded source string.
 * @return TextTokens* Structure with the resulting token list.
 */
TextTokens *TokenizeText(const char *text) {
    if (!text) return NULL;

    TextTokens *tt = (TextTokens*)malloc(sizeof(TextTokens));
    if (!tt) return NULL;
    
    tt->tokens = NULL; 
    tt->count = 0;
    int capacity = 32;
    
    tt->tokens = (char**)malloc(sizeof(char*) * capacity);
    if (!tt->tokens) {
        free(tt);
        return NULL;
    }

    const char *p = text;
    
    while (*p) {
        // Skip whitespace
        if ((unsigned char)*p <= 127 && isspace((unsigned char)*p)) {
            p++;
            continue;
        }

        // Punctuation handling (ASCII)
        if ((unsigned char)*p <= 127 && ispunct((unsigned char)*p)) {
            char *token = (char*)malloc(2);
            if (token) {
                token[0] = *p;
                token[1] = '\0';
                
                if (tt->count >= capacity) {
                    capacity *= 2;
                    char **newTokens = (char**)realloc(tt->tokens, sizeof(char*) * capacity);
                    if (!newTokens) { free(token); break; }
                    tt->tokens = newTokens;
                }
                tt->tokens[tt->count++] = token;
            }
            p++;
            continue;
        }

        // Word / Multi-byte sequence handling
        const char *start = p;
        while (*p) {
            unsigned char c = (unsigned char)*p;
            // Stop at delimiters
            if (c <= 127 && (isspace(c) || ispunct(c))) {
                break;
            }
            p++; // Keep moving (including over UTF-8 high-bit chars)
        }

        int len = p - start;
        if (len > 0) {
            char *token = (char*)malloc(len + 1);
            if (token) {
                memcpy(token, start, len);
                token[len] = '\0';

                if (tt->count >= capacity) {
                    capacity *= 2;
                    char **newTokens = (char**)realloc(tt->tokens, sizeof(char*) * capacity);
                    if (!newTokens) { free(token); break; }
                    tt->tokens = newTokens;
                }
                
                tt->tokens[tt->count++] = token;
            }
        }
    }

    return tt;
}

/**
 * @brief Deallocates all memory associated with a TextTokens structure.
 */
void FreeTextTokens(TextTokens *tokens) {
    if (!tokens) return;
    
    if (tokens->tokens) {
        for (int i = 0; i < tokens->count; i++) {
            if (tokens->tokens[i]) free(tokens->tokens[i]);
        }
        free(tokens->tokens);
    }
    free(tokens);
}
