/**
 * @file main.c
 * @brief Main application entry point for the Machine Translation Evaluation Pipeline.
 * 
 * This program orchestrates the complete MT evaluation pipeline:
 * 1. Loading data from a CSV file.
 * 2. Preprocessing text (cleaning, trimming).
 * 3. Computing evaluation metrics (BLEU, METEOR).
 * 4. Exporting the results back to a CSV.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mt_pipeline.h"
#include "csv_reader.h"
#include "csv_writer.h"
#include "preprocess.h"
#include "scoring.h"
#include "scoring/meteor.h"

/**
 * @brief Orchestrates the execution of the evaluation pipeline.
 * 
 * @param inputFile Path to the input CSV containing source, reference, and MT outputs.
 * @param outputFile Path where the results with computed scores will be saved.
 */
void AppRun(const char *inputFile, const char *outputFile) {
    printf("[Pipeline] Initializing...\n");
    printf("[Pipeline] Input:  %s\n", inputFile);
    printf("[Pipeline] Output: %s\n", outputFile);

    // Step 1: Read CSV input
    PipelineData *data = CsvRead(inputFile);
    if (!data) {
        fprintf(stderr, "[Error] Failed to read input file.\n");
        return;
    }
    printf("[Pipeline] Loaded %d entries with %d MT systems.\n", data->numRows, data->numMtSystems);

    // Step 2: Preprocess text
    if (PreprocessPipeline(data) != MT_SUCCESS) {
        fprintf(stderr, "[Error] Preprocessing failed.\n");
        PipelineDataFree(data);
        return;
    }
    printf("[Pipeline] Preprocessing complete.\n");

    // Step 3: Compute evaluation scores
    // Each of these functions is parallelized via OpenMP where profitable.
    ComputeBleuScores(data);
    ComputeMeteorScores(data);
    ComputeCometScores(data);

    // Step 4: Write results
    if (CsvWriteResults(outputFile, data) != MT_SUCCESS) {
        fprintf(stderr, "[Error] Failed to write results to output file.\n");
    } else {
        printf("[Pipeline] Results successfully exported to %s\n", outputFile);
    }

    // Step 5: Resource cleanup
    PipelineDataFree(data);
    printf("[Pipeline] Execution finished successfully.\n");
}

/**
 * @brief Entry point of the application.
 * 
 * @param argc Count of command line arguments.
 * @param argv Command line arguments (expecting input and output paths).
 * @return int 0 on success, 1 on error.
 */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_csv> <output_csv>\n", argv[0]);
        return 1;
    }

    // Initialize scoring engines (e.g., loading IndoWordNet for METEOR)
    if (InitMeteor("data/indowordnet") != 0) {
        fprintf(stderr, "[Warning] METEOR initialization failed. Scores might be zero.\n");
    }

    AppRun(argv[1], argv[2]);

    // Final global resource teardown
    CleanupMeteor();
    
    return 0;
}
