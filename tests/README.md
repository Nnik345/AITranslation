# Tests Directory

This directory contains test programs for validating components of the MT evaluation pipeline.

## Test Programs

### test_stemmer.c
Validates the Hindi and Tamil Snowball stemmers.

**Compilation:**
```bash
gcc -Iinclude -Isrc/preprocessing/stemmer tests/test_stemmer.c \
    src/preprocessing/stemmer_wrapper.c \
    src/preprocessing/stemmer/api.c \
    src/preprocessing/stemmer/utilities.c \
    src/preprocessing/stemmer/stem_UTF_8_tamil.c \
    src/preprocessing/stemmer/stem_UTF_8_hindi.c \
    -o test_stemmer
```

**Usage:**
```bash
./test_stemmer      # Linux/Mac
test_stemmer.exe    # Windows
```

**What it tests:**
- Tamil stemmer with various grammatical suffixes
- Hindi stemmer with various grammatical suffixes
- UTF-8 encoding handling
- Stemmer API correctness
