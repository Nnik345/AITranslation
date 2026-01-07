/**
 * @file csv_writer.h
 * @brief CSV file writing interface
 * 
 * Provides functions to write MT evaluation results to CSV files
 * with proper quoting and score formatting.
 */

#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "mt_pipeline.h"

/**
 * @brief Write evaluation results to CSV file
 * 
 * Generates dynamic headers based on number of MT systems.
 * Format: Source, MT1, MT1_Bleu, MT1_Meteor, MT1_Comet, ..., Reference
 * 
 * @param filePath Path to output CSV file
 * @param data Pipeline data with computed scores
 * @return MT_SUCCESS on success, error code otherwise
 */
int CsvWriteResults(const char *filePath, const PipelineData *data);

#endif
