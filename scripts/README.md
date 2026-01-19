# Python Scripts Directory

This directory contains utility scripts for the MT evaluation pipeline.

## Scripts

### compute_comet.py
Computes COMET scores using Unbabel's `wmt22-comet-da` model.

**Usage:**
```bash
python scripts/compute_comet.py <input_json> <output_json>
```

Called automatically by the C pipeline via subprocess.

**Requirements:**
- unbabel-comet>=2.0.0
- torch>=2.0.0
- transformers>=4.30.0

---

### import_indowordnet.py
Downloads and exports IndoWordNet synsets for Hindi and Tamil.

**Usage:**
```bash
python scripts/import_indowordnet.py
```

**Output:**
- `data/indowordnet/hi_synsets.txt`
- `data/indowordnet/ta_synsets.txt`

**Requirements:**
- pyiwn>=0.0.5

**Note:** Run this once before using METEOR scoring to generate synonym data.
