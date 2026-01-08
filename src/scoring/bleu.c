/**
 * @file bleu.c
 * @brief BLEU (Bilingual Evaluation Understudy) metric implementation
 * 
 * This file implements the BLEU score calculation for evaluating machine
 * translation quality. BLEU measures the similarity between candidate
 * translations and reference translations using N-gram precision and
 * brevity penalty.
 * 
 * Reference: Papineni et al. (2002) "BLEU: a Method for Automatic 
 * Evaluation of Machine Translation"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "scoring.h"

#define MAX_NGRAM 4  // Maximum N-gram size (1-gram to 4-gram)

/**
 * @brief Compare two strings for equality
 * @param s1 First string
 * @param s2 Second string
 * @return 1 if equal, 0 otherwise
 */
static int StrEquals(const char *s1, const char *s2) {
    return strcmp(s1, s2) == 0;
}

/**
 * @brief Count occurrences of a specific N-gram in tokenized text
 * 
 * This function searches for all occurrences of a given N-gram within
 * the tokenized text. Uses a naive O(N^2) search which is acceptable
 * for typical sentence lengths.
 * 
 * @param tokens Tokenized text to search in
 * @param n Size of the N-gram
 * @param ngram Array of n tokens representing the N-gram to find
 * @return Number of times the N-gram appears in tokens
 */
static int CountNgramFrequency(TextTokens *tokens, int n, char **ngram) {
    if (tokens->count < n) return 0;
    
    int count = 0;
    for (int i = 0; i <= tokens->count - n; i++) {
        int match = 1;
        for (int j = 0; j < n; j++) {
            if (!StrEquals(tokens->tokens[i + j], ngram[j])) {
                match = 0;
                break;
            }
        }
        if (match) count++;
    }
    return count;
}

/**
 * @brief Compute BLEU score for a candidate translation against a reference
 * 
 * Implements the BLEU metric which combines:
 * 1. Modified N-gram precision (1-gram through 4-gram)
 * 2. Brevity penalty for short translations
 * 
 * Formula: BLEU = BP * exp(sum(w_n * log(p_n)))
 * where:
 *   - BP is the brevity penalty
 *   - p_n is the modified precision for n-grams
 *   - w_n is the weight (1/4 for each n-gram size)
 * 
 * @param candidate The machine translation to evaluate
 * @param reference The human reference translation
 * @return BLEU score between 0.0 and 1.0 (0 if any N-gram precision is 0)
 */
double ComputeBleu(const char *candidate, const char *reference) {
    if (!candidate || !reference) return 0.0;

    // Tokenize both strings
    TextTokens *candTokens = TokenizeText(candidate);
    TextTokens *refTokens = TokenizeText(reference);

    // Handle empty or invalid inputs
    if (!candTokens || !refTokens || candTokens->count == 0 || refTokens->count == 0) {
        FreeTextTokens(candTokens);
        FreeTextTokens(refTokens);
        return 0.0;
    }

    double logSum = 0.0;
    int ngramsFound = 0;

    // Compute modified precision for N-grams (N=1 to 4)
    for (int n = 1; n <= MAX_NGRAM; n++) {
        // Skip if sentence is too short for this N-gram size
        if (candTokens->count < n) continue;

        long totalClippedCount = 0;
        long totalCandCount = candTokens->count - n + 1;
        if (totalCandCount <= 0) totalCandCount = 1;

        // Track which N-grams we've already counted to avoid duplicates
        int *visited = calloc(totalCandCount, sizeof(int));
        
        // For each unique N-gram in the candidate
        for (int i = 0; i < totalCandCount; i++) {
            if (visited[i]) continue;
            
            char **currentNgram = &candTokens->tokens[i];
            
            // Count occurrences in candidate (mark as visited)
            int countCand = 0;
            for (int k = i; k < totalCandCount; k++) {
                if (visited[k]) continue;
                
                // Check if N-grams match
                int match = 1;
                for (int x = 0; x < n; x++) {
                    if (!StrEquals(candTokens->tokens[i + x], candTokens->tokens[k + x])) {
                        match = 0;
                        break;
                    }
                }
                
                if (match) {
                    countCand++;
                    visited[k] = 1;
                }
            }
            
            // Count occurrences in reference
            int countRef = CountNgramFrequency(refTokens, n, currentNgram);
            
            // Apply clipping: min(count_cand, count_ref)
            int clipped = (countCand < countRef) ? countCand : countRef;
            totalClippedCount += clipped;
        }
        
        free(visited);

        // Calculate precision for this N-gram size
        double precision = 0.0;
        if (totalCandCount > 0) {
            precision = (double)totalClippedCount / totalCandCount;
        }
        
        // Add to geometric mean (or set to 0 if any precision is 0)
        if (precision > 0) {
            logSum += (1.0 / MAX_NGRAM) * log(precision);
            ngramsFound++;
        } else {
            // Strict BLEU: zero precision means zero score
            logSum = -INFINITY;
        }
    }

    // Calculate brevity penalty
    double bp = 1.0;
    int c = candTokens->count;  // Candidate length
    int r = refTokens->count;   // Reference length
    
    if (c < r) {
        bp = exp(1.0 - (double)r / c);
    }
    
    // Final BLEU score: BP * geometric_mean(precisions)
    double score = bp * exp(logSum);
    
    // Cleanup
    FreeTextTokens(candTokens);
    FreeTextTokens(refTokens);

    return score;
}

/**
 * @brief Compute BLEU scores for all entries in the pipeline data
 * 
 * This function processes all MT systems for all rows in parallel using OpenMP.
 * Each MT output is compared against its reference translation to compute
 * a BLEU score. METEOR and COMET scores are set to 0.0 (placeholders).
 * 
 * @param data Pipeline data containing all entries and MT systems
 */
void ComputeBleuScores(PipelineData *data) {
    if (!data) return;
    
    printf("[%s] Computing BLEU scores for %d rows in parallel...\n", __func__, data->numRows);
    
    // Parallelize across rows using OpenMP
    #pragma omp parallel for
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        
        // Compute BLEU for each MT system against the reference
        for (int j = 0; j < data->numMtSystems; j++) {
            e->bleuScores[j] = ComputeBleu(e->mtOutputs[j], e->reference);
            
            // Placeholders for future metrics
            e->meteorScores[j] = 0.0;
            e->cometScores[j] = 0.0;
        }
    }
}
