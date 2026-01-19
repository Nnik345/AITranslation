/**
 * @file stemmer.h
 * @brief Simplified API for Tamil and Hindi stemming using Snowball stemmers
 * 
 * This header provides a clean, handle-based interface to the Snowball stemming
 * library for Tamil and Hindi languages. The stemmers reduce words to their root
 * forms by removing grammatical suffixes.
 * 
 * @example
 * ```c
 * Stemmer* s = StemmerCreate(LANG_TAMIL);
 * const char* stem = StemmerStemWord(s, "கல்வியின்");
 * printf("Stem: %s\n", stem); // Output: கல்
 * StemmerDelete(s);
 * ```
 * 
 * @note The stemmed word is owned by the Stemmer handle and is valid until
 *       the next call to StemmerStemWord() or StemmerDelete().
 */

#ifndef STEMMER_H
#define STEMMER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Supported stemmer languages
 */
typedef enum {
    LANG_TAMIL,  /**< Tamil language stemmer */
    LANG_HINDI   /**< Hindi language stemmer */
} StemmerLanguage;

/**
 * @brief Opaque stemmer handle
 * 
 * This structure encapsulates the Snowball stemmer environment and language
 * configuration. Users should treat this as an opaque handle.
 */
typedef struct Stemmer Stemmer;

/**
 * @brief Initialize a stemmer for the specified language
 * 
 * Creates and initializes a new stemmer instance for the given language.
 * The stemmer must be freed with StemmerDelete() when no longer needed.
 * 
 * @param lang Language to use (LANG_TAMIL or LANG_HINDI)
 * @return Pointer to stemmer handle on success, NULL on failure (out of memory)
 * 
 * @see StemmerDelete()
 */
Stemmer* StemmerCreate(StemmerLanguage lang);

/**
 * @brief Stem a word using the provided stemmer handle
 * 
 * Reduces the input word to its root form by removing grammatical suffixes.
 * The input word must be in UTF-8 encoding and should be lowercase for best results.
 * 
 * @param s Stemmer handle created by StemmerCreate()
 * @param word The word to stem (UTF-8 encoded, null-terminated)
 * @return Pointer to the stemmed word (UTF-8, null-terminated), or NULL on error
 * 
 * @warning The returned pointer is owned by the stemmer and will be invalidated
 *          on the next call to StemmerStemWord() or StemmerDelete().
 *          Copy the result if you need to preserve it.
 * 
 * @note The stemmer expects composed Unicode (NFC/NFKC) and lowercase input.
 * 
 * @see StemmerCreate()
 */
const char* StemmerStemWord(Stemmer* s, const char* word);

/**
 * @brief Free the stemmer handle and release all resources
 * 
 * Destroys the stemmer instance and frees all associated memory.
 * After calling this function, the stemmer handle must not be used.
 * 
 * @param s Stemmer handle to delete (can be NULL, in which case this is a no-op)
 * 
 * @see StemmerCreate()
 */
void StemmerDelete(Stemmer* s);

#ifdef __cplusplus
}
#endif

#endif // STEMMER_H
