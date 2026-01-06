#ifndef CSV_READER_H
#define CSV_READER_H

#include "mt_pipeline.h"

/*
 * Reads the CSV file and populates PipelineData.
 * Automatically detects the number of MT systems based on header columns.
 * Expected header: Source, MT1, ... MTn, Reference
 */
PipelineData *CsvRead(const char *filePath);
void PipelineDataFree(PipelineData *data);

#endif
