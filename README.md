# Machine Translation Evaluation Pipeline

A high-performance C-based pipeline for evaluating Machine Translation (MT) systems using industry-standard metrics like BLEU and METEOR. This project is optimized for speed using OpenMP and supports multi-byte UTF-8 character sets for Indian languages.

## Features

### Core Infrastructure
-   **Robust CSV I/O**: RFC 4180 compliant reader/writer handling dynamic numbers of MT systems and complex quoted fields.
-   **Multilingual Support**: First-class support for Hindi and Tamil (UTF-8).
-   **Parallel Processing**: OpenMP-based parallelization for preprocessing and metric computation.
-   **Modular Design**: Clean separation between core logic, I/O, preprocessing, scoring, and utilities.

### Supported Metrics
-   **BLEU Score**:
    -   Modified N-gram precision (1-gram to 4-gram).
    -   Brevity penalty.
    -   Standard punctuation-aware tokenization.
-   **METEOR Score**:
    -   Multi-pass greedy alignment (Exact, Stem, Synonym).
    -   Integrated Snowball stemmers for Hindi and Tamil.
    -   Synonym support via IndoWordNet integration.
-   **COMET Score**:
    -   Neural metric using Unbabel's `wmt22-comet-da` model.
    -   Requires source, MT output, and reference.
    -   GPU acceleration when available, CPU fallback.

## Getting Started

### Prerequisites
-   **Compiler**: GCC/G++ with OpenMP support.
-   **Library**: `libgomp` (usually bundled with GCC).
-   **System**: Linux or Windows (MinGW/MSYS2).
-   **Python**: Python 3.8+ with pip (for COMET metric).

### Python Setup (for COMET)
Install Python dependencies:
```bash
pip install -r requirements.txt
```

**Note**: First run will download the COMET model (~2GB). Requires internet connection.

### Data Preparation
To use the METEOR metric with synonym support, you must have the synset files in `data/indowordnet/`:
- `hi_synsets.txt`
- `ta_synsets.txt`

These can be generated using the provided `import_indowordnet.py` script (requires `pyiwn`).

### Building
Run `make` to compile the project:
```bash
make clean && make
```
This produces the `mt_pipeline` executable.

## Usage

```bash
./mt_pipeline <input_csv> <output_csv>
```

### Input CSV Structure
Header format: `Source,MT1,MT2,...,MTn,Reference`

### Output CSV Structure
Expanded format: `Source,MT1,MT1_Bleu,MT1_Meteor,MT1_Comet,...,Reference`

## Project Structure
```
Machine-Translation-Evaluation/
├── src/
│   ├── core/           # Main application entry
│   ├── io/             # CSV I/O implementation
│   ├── preprocessing/  # Text normalization and Stemmer wrappers
│   ├── scoring/        # BLEU and C++ METEOR engine
│   └── utils/          # String and scoring helper utilities
├── include/            # C/C++ Header files
├── data/
│   ├── indowordnet/    # Synonym data for METEOR
│   ├── input/          # Test data (e.g., sample.csv)
│   └── output/         # Result storage
├── Makefile
└── README.md
```

## References
-   Papineni, K., et al. (2002). "BLEU: a method for automatic evaluation of machine translation."
-   Banerjee, S., & Lavie, A. (2005). "METEOR: An automatic metric for MT evaluation with improved correlation with human judgments."

## License
Research and Educational purposes.