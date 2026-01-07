/**
 * @file main.c
 * @brief Main application entry point for MT evaluation pipeline
 * 
 * Orchestrates the complete pipeline: CSV reading, preprocessing,
 * score computation, and result writing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mt_pipeline.h"
#include "csv_reader.h"
#include "csv_writer.h"
#include "preprocess.h"
#include "scoring.h"

/**
 * @brief Run the complete MT evaluation pipeline
 * @param inputFile Path to input CSV file
 * @param outputFile Path to output CSV file
 */
void AppRun(const char *inputFile, const char *outputFile) {
    printf("Starting MT Evaluation Pipeline...\n");
    printf("Input: %s\n", inputFile);
    printf("Output: %s\n", outputFile);

    // Step 1: Read CSV input
    PipelineData *data = CsvRead(inputFile);
    if (!data) {
        fprintf(stderr, "Error reading input file.\n");
        return;
    }
    printf("Successfully loaded %d rows with %d MT systems.\n", data->numRows, data->numMtSystems);

    // Step 2: Preprocess text (trim whitespace)
    if (PreprocessPipeline(data) != MT_SUCCESS) {
        fprintf(stderr, "Error during preprocessing.\n");
        PipelineDataFree(data);
        return;
    }
    printf("Preprocessing complete.\n");

    // Step 3: Compute evaluation scores
    ComputeScores(data);
    // Future: ComputeMeteor(data);
    // Future: ComputeComet(data);

    // Step 4: Write results to CSV
    if (CsvWriteResults(outputFile, data) != MT_SUCCESS) {
        fprintf(stderr, "Error writing output file.\n");
    } else {
        printf("Results written to %s\n", outputFile);
    }

    // Step 5: Cleanup memory
    PipelineDataFree(data);
    printf("Done.\n");
}

/**
 * @brief Program entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, 1 on error
 */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_csv> <output_csv>\n", argv[0]);
        return 1;
    }

    AppRun(argv[1], argv[2]);
    return 0;
}
