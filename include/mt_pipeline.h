#ifndef MT_PIPELINE_H
#define MT_PIPELINE_H

typedef struct {
    char *source;
    char *mt_output;
    char *reference;

    double bleu_score;
    double meteor_score;
    double comet_score;
} MTEntry;

typedef struct {
    int num_rows;
    int num_mt_systems;

    MTEntry **entries;
} PipelineData;

/* Error codes */
#define MT_SUCCESS                0
#define MT_ERROR_FILE_NOT_FOUND  -1
#define MT_ERROR_INVALID_FORMAT  -2
#define MT_ERROR_MEMORY          -3

#endif /* MT_PIPELINE_H */
