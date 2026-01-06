#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_reader.h"
#include "string_utils.h"

#define LINE_BUF_SIZE 16384

PipelineData *CsvRead(const char *filePath) {
    FILE *f = fopen(filePath, "r");
    if (!f) {
        perror("Error opening input file");
        return NULL;
    }

    char line[LINE_BUF_SIZE];
    
    /* 1. Read Header to detect columns */
    if (!fgets(line, sizeof(line), f)) {
        fprintf(stderr, "File is empty.\n");
        fclose(f);
        return NULL;
    }

    int headerCount = 0;
    char **headerTokens = CsvTokenizeLine(line, &headerCount);
    
    /* Expected: Source, MT1...MTn, Reference -> at least 3 cols */
    if (headerCount < 3) {
        fprintf(stderr, "Invalid header format. Expected at least 'Source, MT..., Reference'. Found %d cols.\n", headerCount);
        CsvFreeTokens(headerTokens, headerCount);
        fclose(f);
        return NULL;
    }

    int numMt = headerCount - 2;
    printf("Detected %d MT systems.\n", numMt);
    
    CsvFreeTokens(headerTokens, headerCount);

    /* 2. Allocate PipelineData */
    PipelineData *data = malloc(sizeof(PipelineData));
    if (!data) { fclose(f); return NULL; }
    
    data->numMtSystems = numMt;
    data->numRows = 0;
    
    // Initial capacity for rows
    int rowCap = 100;
    data->entries = malloc(sizeof(MTEntry*) * rowCap);
    
    /* 3. Read Body */
    while (fgets(line, sizeof(line), f)) {
        // Skip empty lines or pure newline
        if (line[0] == '\0' || line[0] == '\n' || line[0] == '\r') continue;
        
        int colCount = 0;
        char **tokens = CsvTokenizeLine(line, &colCount);
        
        if (colCount != headerCount) {
             // In robust CSV, we might handle slight mismatches or empty trailing cols
             // But strict for now.
             // Warning: fgets includes newline, CsvTokenizeLine handles it? Yes (breaks at \r/\n).
             if (colCount < headerCount) {
                  // Might be okay if trailing fields are empty and logic didn't capture them?
                  // CsvTokenizeLine should be robust.
                  fprintf(stderr, "Warning: Skipping row %d. Expected %d cols, found %d.\n", data->numRows + 2, headerCount, colCount);
                  CsvFreeTokens(tokens, colCount);
                  continue;
             }
        }

        if (data->numRows >= rowCap) {
            rowCap *= 2;
            MTEntry **newEntries = realloc(data->entries, sizeof(MTEntry*) * rowCap);
            if (!newEntries) {
                // Allocation failed. Clean up everything.
                CsvFreeTokens(tokens, colCount);
                PipelineDataFree(data); 
                return NULL;
            }
            data->entries = newEntries;
        }

        MTEntry *e = malloc(sizeof(MTEntry));
        // Source is index 0
        e->source = StrDup(tokens[0]);
        // Reference is index last (headerCount - 1)
        e->reference = StrDup(tokens[headerCount - 1]);
        
        // MTs are 1 .. headerCount-2
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
