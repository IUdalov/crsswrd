from openai import OpenAI
import time
import os


_CLIENT = OpenAI()

_WORDS_PATH = os.path.join(os.environ["HOME"], "code/crsswrd/data/ml_words.txt")
_WORDDS_WITH_CLUES = os.path.join(os.environ["HOME"], "code/crsswrd/data/ml_words_with_clues.txt")
_TIMEOUT = 20

def read_words(path):
    with open(path, "r") as f:
        return [line.strip() for line in f.readlines()]

_SYSTEM_PROMPT =  "You are avid ml engineer working on drill making a little drill (crossword) to check the knowledge of ML."


def make_clue(word):
    prompt = f"Please compose a short ai/ml themed clue for word '{word}', do not mention number of letters in clue."
    completion = _CLIENT.chat.completions.create(
        model="gpt-4o-mini",
        messages=[
            {"role": "system", "content": _SYSTEM_PROMPT},
            {"role": "user", "content": prompt}
        ]
    )
    print(prompt, " -> ", completion)
    return completion.choices[0].message.content


def make_clues():
    clues = list()
    for word in read_words(_WORDS_PATH):
        clue = make_clue(word)
        time.sleep(_TIMEOUT)
        clues.append(word + " " + clue.strip('"') + "\n")

    with open(_WORDDS_WITH_CLUES, "w") as f:
        f.writelines(clues)


if __name__ == "__main__":
    make_clues()
