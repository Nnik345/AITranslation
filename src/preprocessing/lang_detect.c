/**
 * @file lang_detect.c
 * @brief Implementation of language detection using Python script (pycld2).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mt_pipeline.h"
#include "json_utils.h" // Assuming we can reuse or need basic json writing

#define TEMP_LANG_INPUT "temp_lang_input.json"
#define TEMP_LANG_OUTPUT "temp_lang_output.json"

// Helper to write string to JSON format (basic escaping)
void write_json_string(FILE *f, const char *s) {
    fputc('"', f);
    while (*s) {
        if (*s == '"') fprintf(f, "\\\"");
        else if (*s == '\\') fprintf(f, "\\\\");
        else if (*s == '\n') fprintf(f, "\\n");
        else fputc(*s, f);
        s++;
    }
    fputc('"', f);
}

void DetectLanguages(PipelineData *data) {
    if (!data || data->numRows == 0) return;

    printf("[LangDetect] Detecting languages for %d entries...\n", data->numRows);

    // 1. Write references to JSON
    FILE *f = fopen(TEMP_LANG_INPUT, "w");
    if (!f) {
        fprintf(stderr, "[LangDetect] Error: Could not open %s for writing\n", TEMP_LANG_INPUT);
        return;
    }

    fprintf(f, "[");
    for (int i = 0; i < data->numRows; i++) {
        if (i > 0) fprintf(f, ",");
        MTEntry *e = data->entries[i];
        if (e && e->reference) {
            write_json_string(f, e->reference);
        } else {
            fprintf(f, "\"\"");
        }
    }
    fprintf(f, "]");
    fclose(f);

    // 2. Call Python script
    char cmd[512];
    #ifdef _WIN32
        sprintf(cmd, "venv\\Scripts\\python.exe scripts/detect_lang.py");
    #else
        sprintf(cmd, "venv/bin/python scripts/detect_lang.py");
    #endif
    
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "[LangDetect] Error: Python script execution failed (code %d)\n", ret);
        return;
    }

    // 3. Read JSON output
    // Simple parser: assumes ["code1", "code2", ...] format
    FILE *fin = fopen(TEMP_LANG_OUTPUT, "r");
    if (!fin) {
        fprintf(stderr, "[LangDetect] Error: Could not open %s for reading\n", TEMP_LANG_OUTPUT);
        return;
    }

    char buffer[1024]; // Temp buffer for reading codes
    int rowIdx = 0;
    
    // Skip opening bracket
    int c;
    while ((c = fgetc(fin)) != EOF) {
        if (c == '[') break;
    }

    while (rowIdx < data->numRows) {
        // Read next string
        // Skip until quote
        while ((c = fgetc(fin)) != EOF) {
            if (c == '"') break;
            if (c == ']') goto done; // End of list
        }
        if (c == EOF) break;

        // Read content
        int len = 0;
        while ((c = fgetc(fin)) != EOF) {
            if (c == '"') {
                buffer[len] = '\0';
                break;
            }
            if (len < sizeof(buffer) - 1) {
                buffer[len++] = (char)c;
            }
        }

        // Store in detected lang
        if (data->entries[rowIdx]) {
            strncpy(data->entries[rowIdx]->lang, buffer, 15);
            data->entries[rowIdx]->lang[15] = '\0';
        }
        
        rowIdx++;
        
        // Skip comma
        while ((c = fgetc(fin)) != EOF) {
            if (c == ',') break;
            if (c == ']') goto done;
        }
    }

done:
    fclose(fin);
    
    // Cleanup
    remove(TEMP_LANG_INPUT);
    remove(TEMP_LANG_OUTPUT);
    
    printf("[LangDetect] Success.\n");
}
