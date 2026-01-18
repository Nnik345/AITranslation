import sys
import os
import io

# Force UTF-8 encoding for all file operations (Fixes Windows 'charmap' error)
if sys.platform == "win32":
    import builtins

    original_open = builtins.open

    def utf8_open(*args, **kwargs):
        # Determine mode
        mode = kwargs.get("mode", args[1] if len(args) > 1 else "r")
        if "encoding" not in kwargs and "b" not in mode:
            kwargs["encoding"] = "utf-8"
        return original_open(*args, **kwargs)

    builtins.open = utf8_open

import pyiwn


def export_synsets(lang_code, output_file):
    print(f"--- Processing {lang_code} ---")
    try:
        # iwn.download()  # Only needed if first-time run fails
        iwn = pyiwn.IndoWordNet(lang_code)

        all_synsets = iwn.all_synsets()
        print(f"Found {len(all_synsets)} synsets. Exporting...")

        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        with open(output_file, "w", encoding="utf-8") as f:
            for synset in all_synsets:
                # Extract all unique synonyms in this synset
                # Replace spaces with underscores for multi-word expressions (MWEs)
                lemmas = sorted(
                    list(
                        set(
                            [
                                lemma.name().replace(" ", "_")
                                for lemma in synset.lemmas()
                            ]
                        )
                    )
                )
                if lemmas:
                    f.write(" ".join(lemmas) + "\n")
        print(f"Successfully saved to: {output_file}")
    except Exception as e:
        print(f"Error: {e}")
        print(
            "Tip: If download fails, try running 'python -c \"import pyiwn; pyiwn.download()\"' first."
        )


if __name__ == "__main__":
    # Absolute path to your project data folder
    base_dir = r"d:\Research\Machine-Translation-Evaluation\data\indowordnet"

    export_synsets(pyiwn.Language.HINDI, os.path.join(base_dir, "hi_synsets.txt"))
    export_synsets(pyiwn.Language.TAMIL, os.path.join(base_dir, "ta_synsets.txt"))
