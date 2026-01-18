/**
 * @file csv_reader.c
 * @brief Implementation of CSV reading utilities for the MT pipeline.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_reader.h"
#include "string_utils.h"

#define LINE_BUF_SIZE 65536 // Increased buffer size for long sentences

/**
 * @brief Reads a CSV file and populates the PipelineData structure.
 * 
 * Expectations:
 * - Header row: Source, MT1, MT2, ..., Reference
 * - Encoding: UTF-8
 * 
 * @param filePath Path to the input CSV file.
 * @return PipelineData* Pointer to loaded data, or NULL on failure.
 */
PipelineData *CsvRead(const char *filePath) {
    FILE *f = fopen(filePath, "r");
    if (!f) {
        perror("[IO Error] Could not open input CSV");
        return NULL;
    }

    char line[LINE_BUF_SIZE];
    
    // 1. Process Header
    if (!fgets(line, sizeof(line), f)) {
        fprintf(stderr, "[IO Warning] Input CSV is empty.\n");
        fclose(f);
        return NULL;
    }

    int headerCount = 0;
    char **headerTokens = CsvTokenizeLine(line, &headerCount);
    
    if (headerCount < 3) {
        fprintf(stderr, "[IO Error] Invalid CSV format. Need at least (Source, MT, Reference).\n");
        CsvFreeTokens(headerTokens, headerCount);
        fclose(f);
        return NULL;
    }

    // MT systems are columns between 'Source' and 'Reference'
    int numMt = headerCount - 2;
    CsvFreeTokens(headerTokens, headerCount);

    // 2. Initialize Data Container
    PipelineData *data = malloc(sizeof(PipelineData));
    if (!data) { fclose(f); return NULL; }
    
    data->numMtSystems = numMt;
    data->numRows = 0;
    
    int rowCap = 128; // Initial row capacity
    data->entries = malloc(sizeof(MTEntry*) * rowCap);
    
    // 3. Read Data Rows
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\0' || line[0] == '\n' || line[0] == '\r') continue;
        
        int colCount = 0;
        char **tokens = CsvTokenizeLine(line, &colCount);
        
        if (colCount < headerCount) {
            fprintf(stderr, "[IO Warning] Skipping malformed row %d (col mismatch).\n", data->numRows + 2);
            CsvFreeTokens(tokens, colCount);
            continue;
        }

        // Dynamic resizing of entry array
        if (data->numRows >= rowCap) {
            rowCap *= 2;
            MTEntry **newEntries = realloc(data->entries, sizeof(MTEntry*) * rowCap);
            if (!newEntries) {
                CsvFreeTokens(tokens, colCount);
                PipelineDataFree(data); 
                fclose(f);
                return NULL;
            }
            data->entries = newEntries;
        }

        // Populate Entry
        MTEntry *e = malloc(sizeof(MTEntry));
        e->source = StrDup(tokens[0]);
        e->reference = StrDup(tokens[headerCount - 1]);
        
        e->mtOutputs = malloc(sizeof(char*) * numMt);
        e->bleuScores = calloc(numMt, sizeof(double));
        e->meteorScores = calloc(numMt, sizeof(double));
        e->cometScores = calloc(numMt, sizeof(double));

        for (int i = 0; i < numMt; i++) {
            e->mtOutputs[i] = StrDup(tokens[i+1]);
        }
        
        data->entries[data->numRows++] = e;
        CsvFreeTokens(tokens, colCount);
    }

    fclose(f);
    return data;
}

/**
 * @brief Deep free of PipelineData and all its entries.
 */
void PipelineDataFree(PipelineData *data) {
    if (!data) return;

    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        if(e) {
            free(e->source);
            free(e->reference);
            if (e->mtOutputs) {
                for (int j = 0; j < data->numMtSystems; j++) free(e->mtOutputs[j]);
                free(e->mtOutputs);
            }
            free(e->bleuScores);
            free(e->meteorScores);
            free(e->cometScores);
            free(e);
        }
    }
    free(data->entries);
    free(data);
}
