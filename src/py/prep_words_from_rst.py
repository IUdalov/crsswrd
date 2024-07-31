import os
import re

_DATASET_PATH = os.path.join(os.path.abspath(""), "../../data/ml-glossary/docs")

def _is_word(w):
    return re.match("^[A-Za-z]{3,}$", w) is not None

def _read_words(path):
    print(f"Reading from {path}")
    with open(path, "r") as f:
        for line in f.readlines():
            for maybe_word in line.lower().strip().split(" "):
                if _is_word(maybe_word):
                    yield maybe_word

def process_dataset():
    words = set()
    for filename in os.listdir(_DATASET_PATH):
        if os.path.splitext(filename)[1] != ".rst":
            continue

        path = os.path.join(_DATASET_PATH, filename)
        for word in _read_words(path):
            words.add(word)

    for w in words:
        print(w)
    print(f"Total {len(words)}")
    with open(f"{os.path.abspath('')}/../../data/ml_words_raw.txt", "w") as f:
        f.write("\n".join(words))


if __name__ == "__main__":
    process_dataset()