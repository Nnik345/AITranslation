/**
 * @file scoring.h
 * @brief Scoring metrics and tokenization interfaces
 * 
 * Provides interfaces for BLEU score calculation and text tokenization
 * utilities used by scoring algorithms.
 */

#ifndef SCORING_H
#define SCORING_H

#include "mt_pipeline.h"

/**
 * @brief Tokenized text structure
 * 
 * Holds an array of token strings and the count.
 * Used for N-gram extraction in scoring metrics.
 */
typedef struct {
    char **tokens;  ///< Array of token strings
    int count;      ///< Number of tokens
} TextTokens;

/* Tokenization Utilities (src/utils/score_utils.c) */

/**
 * @brief Tokenize text into words and punctuation
 * @param text UTF-8 encoded input text
 * @return TextTokens structure or NULL on error
 */
TextTokens *TokenizeText(const char *text);

/**
 * @brief Free tokenized text structure
 * @param tokens TextTokens to free
 */
void FreeTextTokens(TextTokens *tokens);

/* Scoring Functions (src/scoring/bleu.c) */

/**
 * @brief Compute BLEU score for candidate vs reference
 * 
 * Implements document-level BLEU (Papineni et al., 2002) with:
 * - Modified N-gram precision (1-4 grams)
 * - Brevity penalty for short translations
 * - No smoothing (strict BLEU)
 * 
 * @param candidate Machine translation output
 * @param reference Human reference translation
 * @return BLEU score between 0.0 and 1.0
 */
double ComputeBleu(const char *candidate, const char *reference);

/**
 * @brief Compute BLEU scores for all entries in pipeline data
 * 
 * Processes all MT systems in parallel using OpenMP.
 * Populates the bleuScores arrays in each MTEntry.
 * 
 * @param data Pipeline data containing all entries
 */
void ComputeBleuScores(PipelineData *data);

/* Scoring Functions (src/scoring/meteor_wrapper.c) */

/**
 * @brief Compute METEOR scores for all entries in pipeline data
 * 
 * Processes all MT systems in parallel using OpenMP.
 * Populates the meteorScores arrays in each MTEntry.
 * Uses Snowball stemmers and IndoWordNet synonyms.
 * 
 * @param data Pipeline data containing all entries
 */
void ComputeMeteorScores(PipelineData *data);

#endif
