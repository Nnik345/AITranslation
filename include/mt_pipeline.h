/**
 * @file mt_pipeline.h
 * @brief Core data structures and definitions for MT evaluation pipeline
 * 
 * Defines the main data structures for storing machine translation entries
 * and pipeline data, along with error codes used throughout the application.
 */

#ifndef MT_PIPELINE_H
#define MT_PIPELINE_H

/**
 * @brief Single machine translation entry
 * 
 * Contains source text, multiple MT system outputs, reference translation,
 * and evaluation scores for each MT system.
 */
typedef struct {
    char *source;        ///< Original source text
    char **mtOutputs;    ///< Array of MT outputs [0..numMtSystems-1]
    char *reference;     ///< Human reference translation
    char lang[16];       ///< Detected language code (e.g., "hi", "ta")

    // Evaluation scores (parallel arrays, one per MT system)
    double *bleuScores;   ///< BLEU scores array
    double *meteorScores; ///< METEOR scores array (placeholder)
    double *cometScores;  ///< COMET scores array (placeholder)
} MTEntry;

/**
 * @brief Pipeline data container
 * 
 * Holds all MT entries and metadata about the evaluation dataset.
 */
typedef struct {
    int numRows;         ///< Number of translation entries
    int numMtSystems;    ///< Number of MT systems being evaluated
    char **mtSystemNames; ///< Original names of MT systems (e.g., "BashaVerse")
    MTEntry **entries;   ///< Array of pointers to MTEntry structures
} PipelineData;

/* Error codes */
#define MT_SUCCESS                0   ///< Operation successful
#define MT_ERROR_FILE_NOT_FOUND  -1   ///< Input file not found
#define MT_ERROR_INVALID_FORMAT  -2   ///< Invalid CSV format
#define MT_ERROR_MEMORY          -3   ///< Memory allocation failure
#define MT_ERROR_ARGS            -4   ///< Invalid command-line arguments

/**
 * @brief Main application entry point
 * @param inputFile Path to input CSV file
 * @param outputFile Path to output CSV file
 */
void AppRun(const char *inputFile, const char *outputFile);

#endif
