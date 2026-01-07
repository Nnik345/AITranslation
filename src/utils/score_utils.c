/**
 * @file score_utils.c
 * @brief Text tokenization utilities for scoring metrics
 * 
 * Provides UTF-8 safe tokenization for Indian languages and other
 * multi-byte character sets. Handles ASCII punctuation separation
 * for proper BLEU score calculation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scoring.h"
#include "string_utils.h"

/**
 * @brief Tokenize text into words and punctuation marks
 * 
 * This tokenizer is UTF-8 aware and handles:
 * - Multi-byte characters (Indian languages, etc.)
 * - ASCII punctuation as separate tokens
 * - Whitespace as delimiters
 * 
 * Example: "Hello, world" -> ["Hello", ",", "world"]
 * 
 * The tokenization follows standard BLEU tokenization where punctuation
 * is separated from words to ensure fair matching.
 * 
 * @param text Input text to tokenize (UTF-8 encoded)
 * @return TextTokens structure containing array of tokens, or NULL on error
 */
TextTokens *TokenizeText(const char *text) {
    if (!text) return NULL;

    // Allocate token structure
    TextTokens *tt = malloc(sizeof(TextTokens));
    if (!tt) return NULL;
    
    tt->tokens = NULL; 
    tt->count = 0;
    int capacity = 16;
    
    // Allocate initial token array
    tt->tokens = malloc(sizeof(char*) * capacity);
    if (!tt->tokens) {
        free(tt);
        return NULL;
    }

    const char *p = text;
    
    while (*p) {
        // 1. Skip ASCII whitespace
        if ((unsigned char)*p <= 127 && isspace((unsigned char)*p)) {
            p++;
            continue;
        }

        // 2. Handle ASCII punctuation as separate tokens
        // This ensures "Hello," becomes ["Hello", ","] for proper matching
        if ((unsigned char)*p <= 127 && ispunct((unsigned char)*p)) {
            char *token = malloc(2);
            token[0] = *p;
            token[1] = '\0';
            
            // Resize array if needed
            if (tt->count >= capacity) {
                capacity *= 2;
                char **newTokens = realloc(tt->tokens, sizeof(char*) * capacity);
                if (!newTokens) {
                    free(token);
                    break;
                }
                tt->tokens = newTokens;
            }
            
            tt->tokens[tt->count++] = token;
            p++;
            continue;
        }

        // 3. Handle word (alphanumeric or multi-byte UTF-8 sequence)
        // Consume all characters until we hit whitespace or punctuation
        const char *start = p;
        while (*p) {
            unsigned char c = (unsigned char)*p;
            
            // Stop at ASCII whitespace or punctuation
            if (c <= 127 && (isspace(c) || ispunct(c))) {
                break;
            }
            
            // Multi-byte UTF-8 characters (>= 0x80) are part of the word
            // Indian language characters fall into this category
            p++;
        }

        // Extract the word token
        int len = p - start;
        if (len > 0) {
            char *token = malloc(len + 1);
            strncpy(token, start, len);
            token[len] = '\0';

            // Resize array if needed
            if (tt->count >= capacity) {
                capacity *= 2;
                char **newTokens = realloc(tt->tokens, sizeof(char*) * capacity);
                if (!newTokens) {
                    free(token);
                    break;
                }
                tt->tokens = newTokens;
            }
            
            tt->tokens[tt->count++] = token;
        }
    }

    return tt;
}

/**
 * @brief Free memory allocated for TextTokens structure
 * 
 * Safely deallocates all tokens and the structure itself.
 * Handles NULL pointers gracefully.
 * 
 * @param tokens TextTokens structure to free
 */
void FreeTextTokens(TextTokens *tokens) {
    if (!tokens) return;
    
    if (tokens->tokens) {
        // Free each individual token string
        for (int i = 0; i < tokens->count; i++) {
            free(tokens->tokens[i]);
        }
        // Free the token array
        free(tokens->tokens);
    }
    
    // Free the structure itself
    free(tokens);
}
