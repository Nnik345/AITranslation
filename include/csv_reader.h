#ifndef CSV_READER_H
#define CSV_READER_H

#include "mt_pipeline.h"

PipelineData *CsvRead(const char *filePath);
void PipelineDataFree(PipelineData *data);

#endif
