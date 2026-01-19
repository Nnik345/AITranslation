# Snowball Stemmers for Tamil and Hindi

This directory contains the Snowball stemming library integration for Tamil and Hindi languages.

## Overview

Stemming is the process of reducing words to their root form by removing grammatical suffixes. For example:
- Tamil: `கல்வியின்` (of education) → `கல்` (root)
- Hindi: `लड़कों` (boys) → `लड़क` (root)

## Files

### Snowball Runtime
- `api.c`, `api.h` - Core Snowball API for environment management
- `header.h` - Common definitions and structures
- `utilities.c` - String manipulation and pattern matching utilities

### Language-Specific Stemmers
- `stem_UTF_8_tamil.c`, `stem_UTF_8_tamil.h` - Tamil stemmer implementation
- `stem_UTF_8_hindi.c`, `stem_UTF_8_hindi.h` - Hindi stemmer implementation

These files are auto-generated from Snowball `.sbl` algorithm definitions and should not be manually edited.

## Usage

See `include/preprocessing/stemmer.h` for the simplified API. Example:

```c
#include "preprocessing/stemmer.h"

Stemmer* s = StemmerCreate(LANG_TAMIL);
const char* stem = StemmerStemWord(s, "கல்வியின்");
printf("Stem: %s\n", stem);  // Output: கல்
StemmerDelete(s);
```

## Notes

- Input words should be UTF-8 encoded and lowercase
- The stemmed result is valid until the next call to `StemmerStemWord()` or `StemmerDelete()`
- Snowball expects composed Unicode (NFC/NFKC normalization)

## Source

These stemmers are from the Snowball project (https://snowballstem.org/), version 3.0.1.
