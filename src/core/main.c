#include <stdio.h>
#include "csv_reader.h"
#include "csv_writer.h"
#include "preprocess.h"

int main(void) {
    const char *inputPath = "data/input/sample.csv";
    const char *outputPath = "data/output/sample_out.csv";

    printf("Reading CSV from: %s\n", inputPath);
    PipelineData *data = CsvRead(inputPath);
    if (!data) {
        fprintf(stderr, "Failed to read input CSV\n");
        return 1;
    }

    printf("Successfully read %d rows with %d MT systems\n", data->numRows, data->numMtSystems);

    printf("Running preprocessing pipeline...\n");
    if (PreprocessPipeline(data) != 0) {
        fprintf(stderr, "Preprocessing failed\n");
        PipelineDataFree(data);
        return 1;
    }

    printf("Writing results to: %s\n", outputPath);
    if (CsvWriteResults(outputPath, data) != 0) {
        fprintf(stderr, "Failed to write output CSV\n");
        PipelineDataFree(data);
        return 1;
    }

    printf("Pipeline completed successfully!\n");
    PipelineDataFree(data);
    return 0;
}
