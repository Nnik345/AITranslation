#include <stdio.h>
#include "preprocess.h"
#include "string_utils.h"

int PreprocessPipeline(PipelineData *data) {
    if (!data) return MT_ERROR_INVALID_FORMAT;

    printf("[%s] Preprocessing %d data rows...\n", __func__, data->numRows);

    // Parallelize the loop using OpenMP
    #pragma omp parallel for
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        
        // Trim Source and Reference
        StrTrim(e->source);
        StrTrim(e->reference);
        
        // Trim MT Outputs
        for (int j = 0; j < data->numMtSystems; j++) {
            StrTrim(e->mtOutputs[j]);
        }
    }

    return MT_SUCCESS;
}
