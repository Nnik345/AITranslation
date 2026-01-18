/**
 * @file meteor_wrapper.c
 * @brief Implementation of parallelized METEOR score computation.
 */

#include <stdio.h>
#include <string.h>
#include <omp.h>
#include "scoring.h"
#include "scoring/meteor.h"

/**
 * @brief Computes METEOR scores for all MT system entries in the pipeline.
 * 
 * This function iterates through each row and each MT system, detects the 
 * language automatically, and calls the high-performance C++ METEOR engine.
 * 
 * @param data Pipeline data to process.
 */
void ComputeMeteorScores(PipelineData *data) {
    if (!data) return;

    // Use OpenMP to parallelize the row processing.
    // METEOR is computationally heavier than BLEU due to alignment and synsets,
    // making parallelism critical here.
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        if (!e) continue;
        
        for (int j = 0; j < data->numMtSystems; j++) {
            // Heuristic Language Detection:
            // "க்" is a very common Tamil character (ka + pulli).
            // This is a simplified check for this specific research context.
            const char* lang = "hi"; 
            if (e->reference && strstr(e->reference, "க்")) {
                lang = "ta";
            }

            e->meteorScores[j] = ComputeMeteorScore(e->mtOutputs[j], e->reference, lang);
        }
    }
}
