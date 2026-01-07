/**
 * @file preprocess.c
 * @brief Text preprocessing implementation
 * 
 * Provides text normalization (whitespace trimming) for all text fields
 * in the MT evaluation pipeline. Uses OpenMP for parallel processing.
 */

#include <stdio.h>
#include "preprocess.h"
#include "string_utils.h"

/**
 * @brief Preprocess all text in pipeline data
 * 
 * Trims whitespace from source, reference, and all MT outputs.
 * Processes rows in parallel using OpenMP for efficiency.
 * 
 * @param data Pipeline data to preprocess
 * @return MT_SUCCESS on success, MT_ERROR_INVALID_FORMAT if data is NULL
 */
int PreprocessPipeline(PipelineData *data) {
    if (!data) return MT_ERROR_INVALID_FORMAT;

    printf("[%s] Preprocessing %d data rows...\n", __func__, data->numRows);

    // Parallelize across rows using OpenMP
    #pragma omp parallel for
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        
        // Trim source and reference text
        StrTrim(e->source);
        StrTrim(e->reference);
        
        // Trim all MT system outputs
        for (int j = 0; j < data->numMtSystems; j++) {
            StrTrim(e->mtOutputs[j]);
        }
    }

    return MT_SUCCESS;
}
