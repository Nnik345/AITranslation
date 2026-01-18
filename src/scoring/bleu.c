/**
 * @file bleu.c
 * @brief Implementation of the BLEU (Bilingual Evaluation Understudy) metric.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "scoring.h"

#define MAX_NGRAM 4  ///< Standard BLEU uses up to 4-grams.

/**
 * @brief Helper for string comparison.
 */
static int StrEquals(const char *s1, const char *s2) {
    return strcmp(s1, s2) == 0;
}

/**
 * @brief Counts the number of times a specific N-gram appears in a token stream.
 * 
 * @param tokens Token stream of the text.
 * @param n Size of the N-gram.
 * @param ngram Array of tokens forming the N-gram.
 * @return int Frequency of the N-gram.
 */
static int CountNgramFrequency(TextTokens *tokens, int n, char **ngram) {
    if (!tokens || tokens->count < n) return 0;
    
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
 * @brief Computes the sentence-level BLEU score.
 * 
 * Implements modified N-gram precision and brevity penalty.
 * 
 * @param candidate Machine translation output string.
 * @param reference Human reference translation string.
 * @return double BLEU score in range [0, 1].
 */
double ComputeBleu(const char *candidate, const char *reference) {
    if (!candidate || !reference) return 0.0;

    TextTokens *candTokens = TokenizeText(candidate);
    TextTokens *refTokens = TokenizeText(reference);

    if (!candTokens || !refTokens || candTokens->count == 0 || refTokens->count == 0) {
        FreeTextTokens(candTokens);
        FreeTextTokens(refTokens);
        return 0.0;
    }

    double logSum = 0.0;
    int validNgrams = 0;

    for (int n = 1; n <= MAX_NGRAM; n++) {
        if (candTokens->count < n) continue;

        long totalClippedCount = 0;
        long totalCandCount = candTokens->count - n + 1;
        
        // Use a temporary bitset to track which candidate N-grams we've already counted
        int *visited = (int*)calloc(totalCandCount, sizeof(int));
        if (!visited) continue;
        
        for (int i = 0; i < totalCandCount; i++) {
            if (visited[i]) continue;
            
            char **currentNgram = &candTokens->tokens[i];
            
            // Count internal frequency in candidate to handle repeated N-grams
            int countCand = 0;
            for (int k = i; k < totalCandCount; k++) {
                if (visited[k]) continue;
                
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
            
            // Count maximum frequency in reference
            int countRef = CountNgramFrequency(refTokens, n, currentNgram);
            
            // Modified Precision: Clip candidate count by reference count
            int clipped = (countCand < countRef) ? countCand : countRef;
            totalClippedCount += clipped;
        }
        
        free(visited);

        double precision = (totalCandCount > 0) ? (double)totalClippedCount / totalCandCount : 0.0;
        
        if (precision > 0) {
            logSum += (1.0 / MAX_NGRAM) * log(precision);
            validNgrams++;
        } else {
            // Strict BLEU: if any N-gram precision is 0, entire score is 0
            logSum = -1e20; // Effectively 0 after exp()
        }
    }

    // Brevity Penalty Calculation
    double bp = 1.0;
    int c = candTokens->count;  
    int r = refTokens->count;   
    
    if (c < r) {
        bp = exp(1.0 - (double)r / c);
    }
    
    double score = bp * exp(logSum);
    
    FreeTextTokens(candTokens);
    FreeTextTokens(refTokens);

    return score;
}

/**
 * @brief Computes BLEU scores for all entries in the pipeline.
 * 
 * Parallelized using OpenMP across rows.
 */
void ComputeBleuScores(PipelineData *data) {
    if (!data) return;
    
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        if (!e) continue;
        
        for (int j = 0; j < data->numMtSystems; j++) {
            e->bleuScores[j] = ComputeBleu(e->mtOutputs[j], e->reference);
        }
    }
}
