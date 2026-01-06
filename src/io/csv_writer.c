#include <stdio.h>
#include <stdlib.h>
#include "csv_writer.h"

static void WriteField(FILE *f, const char *s, int isLast) {
    if (!s) {
        fprintf(f, "%s", isLast ? "\n" : ",");
        return;
    }
    
    // Always quote fields to be safe (simplest robust approach)
    // Escaping double quotes: " -> ""
    fputc('"', f);
    while (*s) {
        if (*s == '"') {
            fputc('"', f);
            fputc('"', f);
        } else {
            fputc(*s, f);
        }
        s++;
    }
    fputc('"', f);
    
    if (isLast) fputc('\n', f);
    else fputc(',', f);
}

int CsvWriteResults(const char *filePath, const PipelineData *data) {
    if (!data) return MT_ERROR_INVALID_FORMAT;

    FILE *f = fopen(filePath, "w");
    if (!f) return MT_ERROR_FILE_NOT_FOUND;

    // 1. Write Header
    // Source, MT1, MT1_Bleu, MT1_Meteor, MT1_Comet, ... , Reference
    fprintf(f, "Source");
    for (int i = 0; i < data->numMtSystems; i++) {
        fprintf(f, ",MT%d,MT%d_Bleu,MT%d_Meteor,MT%d_Comet", i+1, i+1, i+1, i+1);
    }
    fprintf(f, ",Reference\n");

    // 2. Write Rows
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        
        WriteField(f, e->source, 0); // Source,
        
        for (int j = 0; j < data->numMtSystems; j++) {
            WriteField(f, e->mtOutputs[j], 0); // MTj,
            
            // Scores (Direct numeric write, no need to quote usually, but keep CSV structure)
            // %.4f for precision
            fprintf(f, "%.4f,%.4f,%.4f,", e->bleuScores[j], e->meteorScores[j], e->cometScores[j]);
        }
        
        WriteField(f, e->reference, 1); // Reference (Newline)
    }

    fclose(f);
    return MT_SUCCESS;
}
