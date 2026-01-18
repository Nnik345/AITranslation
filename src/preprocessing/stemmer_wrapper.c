/**
 * @file stemmer_wrapper.c
 * @brief C wrapper for the Snowball stemming library (Hindi and Tamil).
 */

#include <stdlib.h>
#include <string.h>
#include "preprocessing/stemmer.h"
#include "header.h"
#include "stem_UTF_8_tamil.h"
#include "stem_UTF_8_hindi.h"

/**
 * @brief Internal stemmer structure wrapping Snowball's environment.
 */
struct Stemmer {
    struct SN_env* snow_env; ///< The Snowball environment handle.
    StemmerLanguage lang;    ///< The language this stemmer is configured for.
};

/**
 * @brief Creates a new Stemmer instance for a specific language.
 * 
 * @param lang Supported language (LANG_HINDI, LANG_TAMIL).
 * @return Stemmer* Handle to the stemmer, or NULL on error.
 */
Stemmer* StemmerCreate(StemmerLanguage lang) {
    Stemmer* s = (Stemmer*)malloc(sizeof(Stemmer));
    if (!s) return NULL;
    
    s->lang = lang;
    if (lang == LANG_TAMIL) {
        s->snow_env = tamil_UTF_8_create_env();
    } else if (lang == LANG_HINDI) {
        s->snow_env = hindi_UTF_8_create_env();
    } else {
        free(s);
        return NULL;
    }
    
    if (!s->snow_env) {
        free(s);
        return NULL;
    }
    return s;
}

/**
 * @brief Stems a single UTF-8 word.
 * 
 * @param s Stemmer handle.
 * @param word Input word.
 * @return const char* Pointer to the internal stemmed buffer (valid until next call).
 */
const char* StemmerStemWord(Stemmer* s, const char* word) {
    if (!s || !word) return NULL;
    
    int len = strlen(word);
    // Load word into Snowball environment
    if (SN_set_current(s->snow_env, len, (const symbol*)word) < 0) return NULL;
    
    // Execute stemming algorithm
    if (s->lang == LANG_TAMIL) {
        tamil_UTF_8_stem(s->snow_env);
    } else if (s->lang == LANG_HINDI) {
        hindi_UTF_8_stem(s->snow_env);
    }
    
    // Ensure null-termination for safe C string usage. 
    // Snowball typically has extra space in env->p.
    s->snow_env->p[s->snow_env->l] = 0; 
    return (const char*)(s->snow_env->p);
}

/**
 * @brief Frees all resources associated with a Stemmer instance.
 * 
 * @param s Stemmer handle to destroy.
 */
void StemmerDelete(Stemmer* s) {
    if (!s) return;
    
    if (s->lang == LANG_TAMIL) {
        tamil_UTF_8_close_env(s->snow_env);
    } else if (s->lang == LANG_HINDI) {
        hindi_UTF_8_close_env(s->snow_env);
    }
    
    free(s);
}
