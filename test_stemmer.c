/**
 * @file test_stemmer.c
 * @brief Test program for Tamil and Hindi stemmers
 * 
 * This program demonstrates the usage of the stemmer API and validates
 * that the stemmers correctly remove grammatical suffixes from sample words.
 * 
 * Compilation:
 *   gcc -Iinclude -Isrc/preprocessing/stemmer test_stemmer.c \
 *       src/preprocessing/stemmer_wrapper.c \
 *       src/preprocessing/stemmer/api.c \
 *       src/preprocessing/stemmer/utilities.c \
 *       src/preprocessing/stemmer/stem_UTF_8_tamil.c \
 *       src/preprocessing/stemmer/stem_UTF_8_hindi.c \
 *       -o test_stemmer
 * 
 * Usage:
 *   ./test_stemmer  (Linux/Mac)
 *   test_stemmer.exe  (Windows)
 */

#include <stdio.h>
#include <string.h>
#include "preprocessing/stemmer.h"

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @brief Test the Tamil stemmer with sample words
 * 
 * Tests various Tamil words with different grammatical suffixes:
 * - Base word
 * - Genitive case marker
 * - Plural marker
 * - Instrumental case + plural
 */
void test_tamil() {
    printf("Testing Tamil Stemmer:\n");
    
    Stemmer* s = StemmerCreate(LANG_TAMIL);
    if (!s) { 
        printf("  ERROR: Failed to create Tamil stemmer\n"); 
        return; 
    }
    
    // Test words with their linguistic features
    const char* words[] = {
        "கல்வி",      // Education (base form)
        "கல்வியின்",  // Of education (genitive case)
        "மலர்கள்",    // Flowers (plural)
        "மலர்களால்"  // By flowers (instrumental case + plural)
    };
    
    for (int i = 0; i < 4; i++) {
        const char* stem = StemmerStemWord(s, words[i]);
        printf("  %s -> %s\n", words[i], stem);
    }
    
    StemmerDelete(s);
    printf("\n");
}

/**
 * @brief Test the Hindi stemmer with sample words
 * 
 * Tests various Hindi words with different grammatical suffixes:
 * - Oblique plural
 * - Feminine plural
 * - Plural marker
 * - Habitual aspect
 */
void test_hindi() {
    printf("Testing Hindi Stemmer:\n");
    
    Stemmer* s = StemmerCreate(LANG_HINDI);
    if (!s) { 
        printf("  ERROR: Failed to create Hindi stemmer\n"); 
        return; 
    }
    
    // Test words with their linguistic features
    const char* words[] = {
        "लड़कों",    // Boys (oblique plural -ओं)
        "लड़कियाँ",  // Girls (feminine plural -ियाँ)
        "किताबें",   // Books (plural -ें)
        "पड़ता"     // Falls/Does (habitual aspect -ता)
    };
    
    for (int i = 0; i < 4; i++) {
        const char* stem = StemmerStemWord(s, words[i]);
        printf("  %s -> %s\n", words[i], stem);
    }
    
    StemmerDelete(s);
    printf("\n");
}

/**
 * @brief Main entry point
 * 
 * Sets up UTF-8 console output on Windows and runs the stemmer tests.
 * 
 * @return 0 on success
 */
int main() {
#ifdef _WIN32
    // Enable UTF-8 output on Windows console for proper display of Indic scripts
    SetConsoleOutputCP(CP_UTF8);
#endif
    
    test_tamil();
    test_hindi();
    
    printf("All tests completed successfully!\n");
    return 0;
}
