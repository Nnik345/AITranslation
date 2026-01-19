#!/usr/bin/env python3
"""
COMET Score Computation Script

This script loads the Unbabel wmt22-comet-da model and computes
sentence-level COMET scores for machine translation evaluation.

Usage:
    python compute_comet.py <input_json> <output_json>
"""

import sys
import json
import os
from typing import List, Dict, Any

try:
    from comet import download_model, load_from_checkpoint
    import torch
except ImportError as e:
    print(f"Error: Required Python packages not installed.", file=sys.stderr)
    print(f"Please run: pip install -r requirements.txt", file=sys.stderr)
    sys.exit(1)


def load_comet_model(model_name: str = "Unbabel/wmt22-comet-da"):
    """
    Load the COMET model with GPU support if available.

    Args:
        model_name: HuggingFace model identifier

    Returns:
        Loaded COMET model
    """
    try:
        # Download model if not cached
        model_path = download_model(model_name)

        # Detect device
        device = "cuda" if torch.cuda.is_available() else "cpu"
        print(f"[COMET] Using device: {device}", file=sys.stderr)

        # Load model
        model = load_from_checkpoint(model_path)
        model.to(device)
        model.eval()

        return model
    except Exception as e:
        print(f"[COMET Error] Failed to load model: {e}", file=sys.stderr)
        raise


def compute_scores(model, data: List[Dict[str, Any]]) -> List[float]:
    """
    Compute COMET scores for a batch of samples.

    Args:
        model: Loaded COMET model
        data: List of dicts with 'src', 'mt', 'ref' keys

    Returns:
        List of COMET scores (one per sample)
    """
    try:
        # COMET expects this exact format
        samples = [
            {"src": item["source"], "mt": item["mt_output"], "ref": item["reference"]}
            for item in data
        ]

        # Compute scores (returns dict with 'scores' key)
        with torch.no_grad():
            output = model.predict(
                samples, batch_size=8, gpus=1 if torch.cuda.is_available() else 0
            )

        return output.scores
    except Exception as e:
        print(f"[COMET Error] Scoring failed: {e}", file=sys.stderr)
        raise


def main():
    if len(sys.argv) != 3:
        print(
            "Usage: python compute_comet.py <input_json> <output_json>", file=sys.stderr
        )
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    try:
        # Read input JSON
        with open(input_file, "r", encoding="utf-8") as f:
            input_data = json.load(f)

        print(f"[COMET] Loading model...", file=sys.stderr)
        model = load_comet_model()

        print(
            f"[COMET] Processing {len(input_data['entries'])} entries...",
            file=sys.stderr,
        )

        # Process each entry (row)
        results = []
        for entry in input_data["entries"]:
            row_idx = entry["row_idx"]
            mt_systems = entry["mt_systems"]

            # Compute scores for all MT systems in this row
            scores = compute_scores(model, mt_systems)

            results.append({"row_idx": row_idx, "mt_scores": scores})

        # Write output JSON
        output_data = {"scores": results}
        with open(output_file, "w", encoding="utf-8") as f:
            json.dump(output_data, f, indent=2)

        print(
            f"[COMET] Successfully computed scores for {len(results)} rows.",
            file=sys.stderr,
        )
        sys.exit(0)

    except FileNotFoundError as e:
        print(f"[COMET Error] File not found: {e}", file=sys.stderr)
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"[COMET Error] Invalid JSON: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"[COMET Error] Unexpected error: {e}", file=sys.stderr)
        import traceback

        traceback.print_exc(file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
