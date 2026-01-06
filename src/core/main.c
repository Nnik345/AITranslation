#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mt_pipeline.h"
#include "csv_reader.h"
#include "csv_writer.h"
#include "preprocess.h"

void AppRun(const char *inputFile, const char *outputFile) {
    printf("Starting MT Evaluation Pipeline...\n");
    printf("Input: %s\n", inputFile);
    printf("Output: %s\n", outputFile);

    /* 1. Read CSV */
    PipelineData *data = CsvRead(inputFile);
    if (!data) {
        fprintf(stderr, "Error reading input file.\n");
        return;
    }
    printf("Successfully loaded %d rows with %d MT systems.\n", data->numRows, data->numMtSystems);

    /* 2. Preprocess */
    if (PreprocessPipeline(data) != MT_SUCCESS) {
        fprintf(stderr, "Error during preprocessing.\n");
        PipelineDataFree(data);
        return;
    }
    printf("Preprocessing complete.\n");

    /* 3. Compute Scores (Stub) */
    printf("Computing scores (STUB)...\n");
    // Placeholder for score computation logic
    // ComputeBleu(data);
    // ComputeMeteor(data);
    // ComputeComet(data);

    /* 4. Write Results */
    if (CsvWriteResults(outputFile, data) != MT_SUCCESS) {
        fprintf(stderr, "Error writing output file.\n");
    } else {
        printf("Results written to %s\n", outputFile);
    }

    /* 5. Cleanup */
    PipelineDataFree(data);
    printf("Done.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_csv> <output_csv>\n", argv[0]);
        return 1;
    }

    AppRun(argv[1], argv[2]);
    return 0;
}
