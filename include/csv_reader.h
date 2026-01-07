/**
 * @file csv_reader.h
 * @brief CSV file reading interface
 * 
 * Provides functions to read MT evaluation data from CSV files
 * with RFC 4180 compliance (quoted fields, commas, newlines).
 */

#ifndef CSV_READER_H
#define CSV_READER_H

#include "mt_pipeline.h"

/**
 * @brief Read CSV file and populate pipeline data
 * 
 * Automatically detects the number of MT systems from the header.
 * Expected format: Source, MT1, MT2, ..., MTn, Reference
 * 
 * @param filePath Path to input CSV file
 * @return Populated PipelineData structure or NULL on error
 */
PipelineData *CsvRead(const char *filePath);

/**
 * @brief Free all memory associated with pipeline data
 * @param data PipelineData structure to free
 */
void PipelineDataFree(PipelineData *data);

#endif
