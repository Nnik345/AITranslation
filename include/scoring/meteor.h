/**
 * @file meteor.h
 * @brief C-compatible interface for the C++ METEOR scoring engine.
 */

#ifndef METEOR_H
#define METEOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mt_pipeline.h"

/**
 * @brief Initialize the global METEOR resources (synonyms, stemmers).
 * @param synonym_dir Directory containing lang_synsets.txt files.
 * @return 0 on success, non-zero on failure.
 */
int InitMeteor(const char* synonym_dir);

/**
 * @brief Compute METEOR score for a candidate and reference sentence.
 * @param candidate UTF-8 candidate string.
 * @param reference UTF-8 reference string.
 * @param lang Language code ("hi" or "ta").
 * @return METEOR score (0.0 to 1.0).
 */
double ComputeMeteorScore(const char* candidate, const char* reference, const char* lang);

/**
 * @brief Cleanup global METEOR resources.
 */
void CleanupMeteor();

#ifdef __cplusplus
}
#endif

#endif // METEOR_H
