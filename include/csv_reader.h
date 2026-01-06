#ifndef CSV_READER_H
#define CSV_READER_H

#include "mt_pipeline.h"

PipelineData *csv_read(const char *filepath);
void pipeline_data_free(PipelineData *data);

#endif
