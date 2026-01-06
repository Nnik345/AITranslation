#include <stdio.h>
#include <string.h>
#include "csv_writer.h"

static void CsvWriteField(FILE *file, const char *field) {
    if (!field) {
        fprintf(file, "\"\"");
        return;
    }

    int needsQuoting = 0;
    for (const char *p = field; *p; p++) {
        if (*p == ',' || *p == '"' || *p == '\n') {
            needsQuoting = 1;
            break;
        }
    }

    if (needsQuoting) {
        fprintf(file, "\"");
        for (const char *p = field; *p; p++) {
            if (*p == '"') {
                fprintf(file, "\"\"");
            } else {
                fputc(*p, file);
            }
        }
        fprintf(file, "\"");
    } else {
        fprintf(file, "%s", field);
    }
}

int CsvWriteResults(const char *filePath, const PipelineData *data) {
    FILE *file = fopen(filePath, "wb");
    if (!file) return MT_ERROR_FILE_NOT_FOUND;

    if (!data) return MT_ERROR_INVALID_FORMAT;

    fprintf(file, "Source");
    for (int j = 0; j < data->numMtSystems; j++) {
        fprintf(file, ",MT%d,MT%d Scores", j + 1, j + 1);
    }
    fprintf(file, ",Ref\n");

    for (int i = 0; i < data->numRows; i++) {
        for (int j = 0; j < data->numMtSystems; j++) {
            MTEntry *entry = &data->entries[i][j];

            if (j == 0) {
                CsvWriteField(file, entry->source);
                fprintf(file, ",");
            }

            CsvWriteField(file, entry->mtOutput);
            fprintf(file, ",");

            fprintf(file, "%.6f|%.6f|%.6f", 
                entry->bleuScore, entry->meteorScore, entry->cometScore);

            if (j < data->numMtSystems - 1) {
                fprintf(file, ",");
            }
        }

        fprintf(file, ",");
        CsvWriteField(file, data->entries[i][0].reference);
        fprintf(file, "\n");
    }

    fclose(file);
    return MT_SUCCESS;
}
