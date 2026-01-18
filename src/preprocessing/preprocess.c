/**
 * @file preprocess.c
 * @brief Implementation of text normalization and preprocessing.
 */

#include <stdio.h>
#include <omp.h>
#include "preprocess.h"
#include "string_utils.h"

/**
 * @brief Normalizes all text fields within the PipelineData structure.
 * 
 * This currently performs whitespace trimming on:
 * - Source text
 * - Reference translation
 * - All machine translation outputs
 * 
 * @param data Data structure to process in-place.
 * @return int MT_SUCCESS on completion, or error code.
 */
int PreprocessPipeline(PipelineData *data) {
    if (!data) return MT_ERROR_INVALID_FORMAT;

    // Parallelize processing across entries for speed improvement on large datasets.
    #pragma omp parallel for
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        if (!e) continue;
        
        // Trim source and reference
        if (e->source) StrTrim(e->source);
        if (e->reference) StrTrim(e->reference);
        
        // Trim all MT outputs
        if (e->mtOutputs) {
            for (int j = 0; j < data->numMtSystems; j++) {
                if (e->mtOutputs[j]) StrTrim(e->mtOutputs[j]);
            }
        }
    }

    return MT_SUCCESS;
}
