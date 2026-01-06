#ifndef STRING_UTILS_H
#define STRING_UTILS_H

char *StrDup(const char *s);
char *StrTrim(char *s);

/* 
 * Splits a CSV line into fields, handling double quotes (RFC 4180).
 * Returns a NULL-terminated array of strings.
 * Sets *count to the number of fields.
 */
char **CsvTokenizeLine(const char *line, int *count);
void CsvFreeTokens(char **tokens, int count);

#endif
