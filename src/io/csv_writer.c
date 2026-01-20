/**
 * @file csv_writer.c
 * @brief Implementation of CSV writing utilities for exporting results.
 */

#include <stdio.h>
#include <stdlib.h>
#include "csv_writer.h"

/**
 * @brief Helper to write a quoted CSV field.
 * 
 * @param f Output file handle.
 * @param s String content.
 * @param isLast Boolean indicating if this is the last column in the row.
 */
static void WriteField(FILE *f, const char *s, int isLast) {
    if (!s) {
        fprintf(f, "%s", isLast ? "\n" : ",");
        return;
    }
    
    // Quote fields to handle commas and preserve formatting.
    // Escapes double quotes by doubling them (Standard RFC 4180).
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

/**
 * @brief Exports computed scores to a new CSV file.
 * 
 * @param filePath Output file destination.
 * @param data PipelineData containing original text and computed scores.
 * @return int MT_SUCCESS or error code.
 */
int CsvWriteResults(const char *filePath, const PipelineData *data) {
    if (!data) return MT_ERROR_INVALID_FORMAT;

    FILE *f = fopen(filePath, "w");
    if (!f) {
        perror("[IO Error] Could not open output file for writing");
        return MT_ERROR_FILE_NOT_FOUND;
    }

    // 1. Write Header
    // Consistent structure: Source, [SystemName, SystemName_Bleu, ...], Reference
    fprintf(f, "Source");
    for (int i = 0; i < data->numMtSystems; i++) {
        const char *sysName = data->mtSystemNames ? data->mtSystemNames[i] : "MT";
        
        fprintf(f, ",");
        WriteField(f, sysName, 0); // System Output Column
        
        // Write Score Headers using original name
        fprintf(f, ",%s_Bleu", sysName);
        fprintf(f, ",%s_Meteor", sysName);
        fprintf(f, ",%s_Comet", sysName);
    }
    fprintf(f, ",Reference\n");

    // 2. Write Data Rows
    for (int i = 0; i < data->numRows; i++) {
        MTEntry *e = data->entries[i];
        
        WriteField(f, e->source, 0); 
        
        for (int j = 0; j < data->numMtSystems; j++) {
            WriteField(f, e->mtOutputs[j], 0); 
            
            // Numeric scores are written with fixed precision.
            fprintf(f, "%.4f,%.4f,%.4f,", 
                    e->bleuScores[j], 
                    e->meteorScores[j], 
                    e->cometScores[j]);
        }
        
        WriteField(f, e->reference, 1); // Reference col + newline
    }

    fclose(f);
    return MT_SUCCESS;
}
