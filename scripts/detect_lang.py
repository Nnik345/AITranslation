"""
Script to detect languages for a batch of text using pycld2.
Input: temp_lang_input.json (list of strings)
Output: temp_lang_output.json (list of language codes)
"""

import sys
import json
import pycld2 as cld2
import os

INPUT_FILE = "temp_lang_input.json"
OUTPUT_FILE = "temp_lang_output.json"


def detect_languages():
    if not os.path.exists(INPUT_FILE):
        print(f"Error: {INPUT_FILE} not found")
        sys.exit(1)

    try:
        with open(INPUT_FILE, "r", encoding="utf-8") as f:
            texts = json.load(f)

        results = []
        for text in texts:
            if not text or not text.strip():
                results.append("unknown")
                continue

            try:
                # isReliable, textBytesFound, details = cld2.detect(text)
                # details is a list of tuples: (name, code, percent, score)
                is_reliable, _, details = cld2.detect(text)

                # Use the top detected language
                lang_code = details[0][1]

                # If unrelaible or unknown, fallback to 'unknown' (or handle downstream)
                if lang_code == "un":
                    results.append("unknown")
                else:
                    results.append(lang_code)

            except Exception as e:
                # Fallback if detection crashes for some reason
                results.append("unknown")

        with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
            json.dump(results, f)

        print("Language detection complete.")

    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    detect_languages()
