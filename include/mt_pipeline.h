#ifndef MT_PIPELINE_H
#define MT_PIPELINE_H

typedef struct {
    char *source;
    char **mtOutputs;   // Array of strings: mtOutputs[0]..mtOutputs[numMtSystems-1]
    char *reference;

    /*
     * For each MT system (index i), we store evaluation scores.
     * These could be parallel arrays or a struct per system.
     * Using parallel arrays for simplicity for now.
     */
    double *bleuScores;   // Array of size numMtSystems
    double *meteorScores; // Array of size numMtSystems
    double *cometScores;  // Array of size numMtSystems
} MTEntry;

typedef struct {
    int numRows;
    int numMtSystems;

    MTEntry **entries; // Array of pointers to MTEntry
} PipelineData;

/* Error codes */
#define MT_SUCCESS                0
#define MT_ERROR_FILE_NOT_FOUND  -1
#define MT_ERROR_INVALID_FORMAT  -2
#define MT_ERROR_MEMORY          -3
#define MT_ERROR_ARGS            -4

/* Function Prototypes for core logic can go here if shared */
void AppRun(const char *inputFile, const char *outputFile);

#endif
