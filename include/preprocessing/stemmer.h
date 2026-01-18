/**
 * @file stemmer.h
 * @brief Simplified API for Tamil and Hindi stemming
 */

#ifndef STEMMER_H
#define STEMMER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LANG_TAMIL,
    LANG_HINDI
} StemmerLanguage;

typedef struct Stemmer Stemmer;

/**
 * @brief Initialize a stemmer for the specified language
 * @param lang Language to use (LANG_TAMIL or LANG_HINDI)
 * @return Pointer to stemmer handle, or NULL on failure
 */
Stemmer* StemmerCreate(StemmerLanguage lang);

/**
 * @brief Stem a word using the provided stemmer handle
 * @param s Stemmer handle created by StemmerCreate
 * @param word The word to stem (UTF-8)
 * @return Pointer to the stemmed word (owned by the stemmer, valid until next call)
 */
const char* StemmerStemWord(Stemmer* s, const char* word);

/**
 * @brief Free the stemmer handle
 * @param s Stemmer handle to delete
 */
void StemmerDelete(Stemmer* s);

#ifdef __cplusplus
}
#endif

#endif // STEMMER_H
