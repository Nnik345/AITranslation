#ifndef MT_PIPELINE_H
#define MT_PIPELINE_H

typedef struct {
    char *source;
    char *mtOutput;
    char *reference;

    double bleuScore;
    double meteorScore;
    double cometScore;
} MTEntry;

typedef struct {
    int numRows;
    int numMtSystems;

    MTEntry **entries;
} PipelineData;

/* Error codes */
#define MT_SUCCESS                0
#define MT_ERROR_FILE_NOT_FOUND  -1
#define MT_ERROR_INVALID_FORMAT  -2
#define MT_ERROR_MEMORY          -3

#endif
