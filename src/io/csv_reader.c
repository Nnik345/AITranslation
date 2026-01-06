#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_reader.h"
#include "string_utils.h"

static char *ParseCsvField(char **position) {
    char *start = *position;
    
    if (*start == '"') {
        start++;
        char *end = start;
        
        while (*end && !(*end == '"' && *(end + 1) != '"')) {
            if (*end == '"' && *(end + 1) == '"') {
                end += 2;
            } else {
                end++;
            }
        }
        
        if (*end == '"') {
            *end = '\0';
            *position = end + 2;
            if (**position == ',') (*position)++;
        }
        
        char *readPos = start;
        char *writePos = start;
        while (*readPos) {
            if (*readPos == '"' && *(readPos + 1) == '"') {
                *writePos++ = '"';
                readPos += 2;
            } else {
                *writePos++ = *readPos++;
            }
        }
        *writePos = '\0';
        
        return start;
    } else {
        char *end = start;
        while (*end && *end != ',' && *end != '\n') {
            end++;
        }
        
        char *field = start;
        if (*end == ',') {
            *end = '\0';
            *position = end + 1;
        } else if (*end == '\n' || *end == '\0') {
            *position = end;
        }
        
        return field;
    }
}

PipelineData *CsvRead(const char *filePath) {
    FILE *file = fopen(filePath, "rb");
    if (!file) return NULL;

    PipelineData *pipelineData = malloc(sizeof(PipelineData));
    if (!pipelineData) {
        fclose(file);
        return NULL;
    }

    char *line = NULL;
    size_t lineLen = 0;
    ssize_t readLen;

    readLen = getline(&line, &lineLen, file);
    if (readLen <= 0) {
        free(line);
        free(pipelineData);
        fclose(file);
        return NULL;
    }

    int commaCount = 0;
    int inQuotes = 0;
    for (size_t i = 0; i < (size_t)readLen; i++) {
        if (line[i] == '"') {
            inQuotes = !inQuotes;
        } else if (line[i] == ',' && !inQuotes) {
            commaCount++;
        }
    }
    pipelineData->numMtSystems = commaCount - 1;

    if (pipelineData->numMtSystems <= 0) {
        free(line);
        free(pipelineData);
        fclose(file);
        return NULL;
    }

    int rowCount = 0;
    while ((readLen = getline(&line, &lineLen, file)) != -1) {
        if (readLen > 0 && line[readLen - 1] == '\n') {
            rowCount++;
        }
    }

    pipelineData->numRows = rowCount;

    pipelineData->entries = malloc(rowCount * sizeof(MTEntry *));
    if (!pipelineData->entries) {
        free(line);
        free(pipelineData);
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < rowCount; i++) {
        pipelineData->entries[i] = malloc(pipelineData->numMtSystems * sizeof(MTEntry));
        if (!pipelineData->entries[i]) {
            for (int j = 0; j < i; j++) {
                free(pipelineData->entries[j]);
            }
            free(pipelineData->entries);
            free(line);
            free(pipelineData);
            fclose(file);
            return NULL;
        }
    }

    rewind(file);
    getline(&line, &lineLen, file);

    int rowIdx = 0;
    while ((readLen = getline(&line, &lineLen, file)) != -1 && rowIdx < rowCount) {
        if (readLen > 0 && line[readLen - 1] == '\n') {
            line[readLen - 1] = '\0';
            readLen--;
        }

        char *lineCopy = StrDup(line);
        if (!lineCopy) {
            free(line);
            PipelineDataFree(pipelineData);
            fclose(file);
            return NULL;
        }

        char *position = lineCopy;
        int colIdx = 0;

        while (colIdx <= pipelineData->numMtSystems && *position) {
            char *field = ParseCsvField(&position);
            char *trimmedField = StrTrim(field);

            if (colIdx == 0) {
                for (int j = 0; j < pipelineData->numMtSystems; j++) {
                    pipelineData->entries[rowIdx][j].source = StrDup(trimmedField);
                }
            } else if (colIdx == pipelineData->numMtSystems + 1) {
                for (int j = 0; j < pipelineData->numMtSystems; j++) {
                    pipelineData->entries[rowIdx][j].reference = StrDup(trimmedField);
                }
            } else {
                int mtIdx = colIdx - 1;
                pipelineData->entries[rowIdx][mtIdx].mtOutput = StrDup(trimmedField);
                pipelineData->entries[rowIdx][mtIdx].bleuScore = 0.0;
                pipelineData->entries[rowIdx][mtIdx].meteorScore = 0.0;
                pipelineData->entries[rowIdx][mtIdx].cometScore = 0.0;
            }

            colIdx++;
        }

        free(lineCopy);
        rowIdx++;
    }

    free(line);
    fclose(file);
    return pipelineData;
}

void PipelineDataFree(PipelineData *data) {
    if (!data) return;

    for (int i = 0; i < data->numRows; i++) {
        for (int j = 0; j < data->numMtSystems; j++) {
            MTEntry *entry = &data->entries[i][j];
            free(entry->source);
            free(entry->mtOutput);
            free(entry->reference);
        }
        free(data->entries[i]);
    }
    free(data->entries);
    free(data);
}
