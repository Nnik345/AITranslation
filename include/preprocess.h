/**
 * @file preprocess.h
 * @brief Text preprocessing interface
 * 
 * Provides text normalization functions for MT evaluation pipeline.
 */

#ifndef PREPROCESS_H
#define PREPROCESS_H

#include "mt_pipeline.h"

/**
 * @brief Preprocess all text in pipeline data
 * 
 * Applies text normalization (whitespace trimming) to all source,
 * reference, and MT output strings. Runs in parallel using OpenMP.
 * 
 * @param data Pipeline data to preprocess
 * @return MT_SUCCESS on success, error code otherwise
 */
int PreprocessPipeline(PipelineData *data);

#endif
