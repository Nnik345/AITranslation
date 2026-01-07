# Machine Translation Evaluation Pipeline

A C-based pipeline for evaluating Machine Translation (MT) systems using metrics like BLEU, METEOR, and COMET. This project supports evaluating multiple MT systems against a reference translation, with specific support for Indian languages (UTF-8).

## Features

### Phase 1: Infrastructure ✅
-   **Robust CSV I/O**: Reads/writes CSV files with RFC 4180 compliance, handling dynamic numbers of MT systems and quoted fields.
-   **Multilingual Support**: Fully supports UTF-8 for Indian languages (Hindi, Tamil, Telugu, Kannada, Bengali, etc.).
-   **Parallel Preprocessing**: Uses OpenMP to parallelize text normalization across data rows.
-   **Extensible Structure**: Modular design with separate Core, I/O, Preprocessing, Scoring, and Utils components.

### Phase 2: BLEU Metric ✅
-   **BLEU Score Calculation**: Implements the standard BLEU metric (Papineni et al., 2002)
    -   Modified N-gram precision (1-gram through 4-gram)
    -   Brevity penalty for short translations
    -   UTF-8 aware tokenization with punctuation separation
-   **Parallel Scoring**: Computes scores in parallel using OpenMP for efficient processing

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
Source,MT1,MT2,...,MTn,Reference
```
-   **Source**: The original source sentence
-   **MT1...MTn**: Output from various translation systems (e.g., ChatGPT, Google Translate, Bashaverse)
-   **Reference**: The human reference translation

Example (`data/input/sample.csv`):
```csv
Source,ChatGPT,Google Translate,Bashaverse,Reference
"Hello, how are you?","नमस्ते, आप कैसे हैं?","हेलो, आप कैसे हैं?","नमस्ते, क्या हाल है?","नमस्ते, आप कैसे हैं?"
```

### Output Format
The output CSV contains the original data plus score columns for each MT system:
```csv
Source,MT1,MT1_Bleu,MT1_Meteor,MT1_Comet,MT2,MT2_Bleu,MT2_Meteor,MT2_Comet,...,Reference
```

Currently, only BLEU scores are computed. METEOR and COMET scores are placeholders (0.0000).

## Project Structure
```
Machine-Translation-Evaluation/
├── src/
│   ├── core/           # Main application logic
│   ├── io/             # CSV Reader and Writer
│   ├── preprocessing/  # Text normalization
│   ├── scoring/        # BLEU implementation
│   └── utils/          # String and tokenization utilities
├── include/            # Header files
├── data/
│   ├── input/          # Sample input data
│   └── output/         # Generated results
├── Makefile
└── README.md
```

## Implementation Details

### BLEU Score
The BLEU (Bilingual Evaluation Understudy) metric measures translation quality by:
1.  **N-gram Precision**: Counting how many word sequences (1-4 words) in the candidate match the reference
2.  **Modified Precision**: Clipping counts to prevent gaming the system
3.  **Brevity Penalty**: Penalizing translations that are too short

Formula: `BLEU = BP × exp(Σ(w_n × log(p_n)))`

### Tokenization
The tokenizer is UTF-8 aware and:
-   Treats multi-byte characters (Indian scripts) as word components
-   Separates ASCII punctuation into individual tokens
-   Uses whitespace as primary delimiter

Example: `"Hello, world"` → `["Hello", ",", "world"]`

### Parallelization
Both preprocessing and scoring use OpenMP to process rows in parallel, utilizing all available CPU cores for faster execution.

## Future Plans
-   Implementation of METEOR metric
-   Implementation of COMET metric
-   Corpus-level BLEU calculation
-   Additional preprocessing options (lowercasing, normalization)

## References
-   Papineni, K., Roukos, S., Ward, T., & Zhu, W. J. (2002). BLEU: a method for automatic evaluation of machine translation. *Proceedings of ACL*.

## License
This project is for research and educational purposes.