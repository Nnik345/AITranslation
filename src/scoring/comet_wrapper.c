/**
 * @file comet_wrapper.c
 * @brief C wrapper for calling Python COMET scoring script via subprocess.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scoring.h"
#include "mt_pipeline.h"

#define COMET_INPUT_FILE "temp_comet_input.json"
#define COMET_OUTPUT_FILE "temp_comet_output.json"
#define COMET_SCRIPT "compute_comet.py"

/**
 * @brief Writes pipeline data to JSON format for COMET script.
 */
static int WriteCometInputJson(const char* filepath, PipelineData *data) {
    FILE *f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "[COMET Error] Could not create input file: %s\n", filepath);
        return -1;
    }

    fprintf(f, "{\n  \"entries\": [\n");
    
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        
        fprintf(f, "    {\n");
        fprintf(f, "      \"row_idx\": %d,\n", i);
        fprintf(f, "      \"mt_systems\": [\n");
        
        for (int j = 0; j < data->numMtSystems; j++) {
            fprintf(f, "        {\n");
            fprintf(f, "          \"mt_idx\": %d,\n", j);
            
            // Escape quotes in JSON strings
            fprintf(f, "          \"source\": \"");
            for (const char *p = e->source; p && *p; p++) {
                if (*p == '"' || *p == '\\') fputc('\\', f);
                fputc(*p, f);
            }
            fprintf(f, "\",\n");
            
            fprintf(f, "          \"mt_output\": \"");
            for (const char *p = e->mtOutputs[j]; p && *p; p++) {
                if (*p == '"' || *p == '\\') fputc('\\', f);
                fputc(*p, f);
            }
            fprintf(f, "\",\n");
            
            fprintf(f, "          \"reference\": \"");
            for (const char *p = e->reference; p && *p; p++) {
                if (*p == '"' || *p == '\\') fputc('\\', f);
                fputc(*p, f);
            }
            fprintf(f, "\"\n");
            
            fprintf(f, "        }%s\n", (j < data->numMtSystems - 1) ? "," : "");
        }
        
        fprintf(f, "      ]\n");
        fprintf(f, "    }%s\n", (i < data->numRows - 1) ? "," : "");
    }
    
    fprintf(f, "  ]\n}\n");
    fclose(f);
    return 0;
}

/**
 * @brief Reads COMET scores from JSON output file.
 */
static int ReadCometOutputJson(const char* filepath, PipelineData *data) {
    FILE *f = fopen(filepath, "r");
    if (!f) {
        fprintf(stderr, "[COMET Error] Could not read output file: %s\n", filepath);
        return -1;
    }

    // Simple JSON parser for our specific format
    char line[4096];
    int current_row = -1;
    int mt_idx = 0;
    
    while (fgets(line, sizeof(line), f)) {
        // Look for "row_idx": <number>
        if (strstr(line, "\"row_idx\"")) {
            sscanf(line, " \"row_idx\": %d", &current_row);
            mt_idx = 0;
        }
        // Look for score values in "mt_scores" array
        else if (current_row >= 0 && current_row < data->numRows) {
            // Try to parse a float from the line
            double score;
            if (sscanf(line, " %lf", &score) == 1) {
                if (mt_idx < data->numMtSystems) {
                    data->entries[current_row]->cometScores[mt_idx] = score;
                    mt_idx++;
                }
            }
        }
    }
    
    fclose(f);
    return 0;
}

/**
 * @brief Computes COMET scores for all entries by calling Python script.
 */
void ComputeCometScores(PipelineData *data) {
    if (!data) return;

    printf("[COMET] Preparing data for scoring...\n");
    
    // Write input JSON
    if (WriteCometInputJson(COMET_INPUT_FILE, data) != 0) {
        fprintf(stderr, "[COMET Warning] Failed to write input. Skipping COMET scoring.\n");
        return;
    }

    // Build command to run Python script
    // Use venv Python if available, otherwise fall back to system python
    char command[512];
    #ifdef _WIN32
    snprintf(command, sizeof(command), 
             "venv\\Scripts\\python.exe %s %s %s 2>&1", 
             COMET_SCRIPT, COMET_INPUT_FILE, COMET_OUTPUT_FILE);
    #else
    snprintf(command, sizeof(command), 
             "venv/bin/python %s %s %s 2>&1", 
             COMET_SCRIPT, COMET_INPUT_FILE, COMET_OUTPUT_FILE);
    #endif
    
    printf("[COMET] Running Python scoring script...\n");
    printf("[COMET] Command: %s\n", command);
    
    // Execute Python script
    int exit_code = system(command);
    
    if (exit_code != 0) {
        fprintf(stderr, "[COMET Warning] Python script failed (exit code: %d). COMET scores set to 0.0\n", exit_code);
        fprintf(stderr, "[COMET Warning] Make sure Python and dependencies are installed: pip install -r requirements.txt\n");
        
        // Clean up and return (scores remain 0.0 from calloc)
        remove(COMET_INPUT_FILE);
        return;
    }

    // Read output JSON
    if (ReadCometOutputJson(COMET_OUTPUT_FILE, data) != 0) {
        fprintf(stderr, "[COMET Warning] Failed to parse output. COMET scores set to 0.0\n");
    } else {
        printf("[COMET] Successfully computed scores for %d rows.\n", data->numRows);
    }

    // Cleanup temp files
    remove(COMET_INPUT_FILE);
    remove(COMET_OUTPUT_FILE);
}
