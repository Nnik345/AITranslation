# Machine Translation Evaluation Pipeline

A C-based pipeline for evaluating Machine Translation (MT) systems using metrics like BLEU, METEOR, and COMET. This project supports evaluating multiple MT systems against a reference translation, with specific support for Indian languages (UTF-8).

## Features (Phase 1)
-   **Robust CSV I/O**: Reads/writes CSV files, handling dynamic numbers of MT systems and RFC 4180 quoted fields.
-   **Multilingual Support**: Fully supports UTF-8 for Indian languages (Hindi, Tamil, Telugu, etc.).
-   **Parallel Preprocessing**: Uses OpenMP to parallelize text normalization across data rows.
-   **Extensible Structure**: Modular design with separate Core, I/O, Preprocessing, and Utils components.

## Building the Project

### Prerequisites
-   GCC (C11 support) with OpenMP support (`libgomp`)
-   Make

### Build Command
Run `make` in the root directory:
```bash
make
```
This generates the `mt_pipeline` executable.

To clean build artifacts:
```bash
make clean
```

## Usage

```bash
./mt_pipeline <input_csv> <output_csv>
```

### Input Format
The input CSV should have the following header structure:
```csv
Source, MT1, MT2, ... MTn, Reference
```
-   **Source**: The original source sentence.
-   **MT1...MTn**: Output from various translation systems (e.g., ChatGPT, Google Translate).
-   **Reference**: The human reference translation.

Example (`data/input/sample.csv`):
```csv
Source,ChatGPT,Google Translate,Bashaverse,Reference
"Hello","नमस्ते","हेलो","प्रणाम","नमस्ते"
```

### Output Format
The output CSV will contain the original data plus score columns for each MT system:
```csv
Source, MT1, MT1_Bleu, MT1_Meteor, MT1_Comet, MT2, ... Reference
```

## Project Structure
-   `src/core/`: Main application logic (`main.c`).
-   `src/io/`: CSV Reader (`csv_reader.c`) and Writer (`csv_writer.c`).
-   `src/preprocessing/`: Text normalization logic (`preprocess.c`).
-   `src/utils/`: String utilities (`string_utils.c`).
-   `include/`: Header files.
-   `data/`: Sample input and output data.

## Future Plans
-   Implementation of BLEU, METEOR, and COMET scoring algorithms.
-   Parallelization of scoring logic using OpenMP.