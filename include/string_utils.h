/**
 * @file string_utils.h
 * @brief String manipulation utilities
 * 
 * Provides string operations and CSV tokenization functions.
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

/**
 * @brief Duplicate a string (safe malloc + strcpy)
 * @param s String to duplicate
 * @return Newly allocated string or NULL on error
 */
char *StrDup(const char *s);

/**
 * @brief Trim whitespace from string in-place
 * @param s String to trim (modified in-place)
 * @return Pointer to trimmed string
 */
char *StrTrim(char *s);

/**
 * @brief Tokenize CSV line into fields
 * 
 * Handles RFC 4180 compliance: quoted fields, escaped quotes,
 * commas within quotes, newlines.
 * 
 * @param line CSV line to tokenize
 * @param count Output parameter for number of fields
 * @return Array of field strings or NULL on error
 */
char **CsvTokenizeLine(const char *line, int *count);

/**
 * @brief Free CSV tokens array
 * @param tokens Array of token strings
 * @param count Number of tokens
 */
void CsvFreeTokens(char **tokens, int count);

#endif
