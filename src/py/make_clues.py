from openai import OpenAI
import tqdm
import time
import os


_CLIENT = OpenAI()
_MODEL = "gpt-4o" #"gpt-4o-mini"
_WORDS_PATH = os.path.join(os.environ["HOME"], "code/crsswrd/data/ml_words.txt")
_WORDS_WITH_CLUES = os.path.join(os.environ["HOME"], "code/crsswrd/data/ml_words_with_clues.txt")
_TIMEOUT = 15

for model in _CLIENT.models.list():
    print(f"- {model.id}")


_SYSTEM_PROMPT =  "You are avid ml engineer working on a little puzzle (crossword) to check the knowledge of ML of your collegues."


def read_words(path):
    with open(path, "r") as f:
        return [line.strip() for line in f.readlines()]


def make_clue(word):
    prompt = f"Please compose a short ai/ml themed clue for word '{word}', do not mention number of letters in the clue."
    completion = _CLIENT.chat.completions.create(
        model=_MODEL,
        messages=[
            {"role": "system", "content": _SYSTEM_PROMPT},
            {"role": "user", "content": prompt}
        ]
    )

    tqdm.tqdm.write(f"{prompt} -> {completion}")
    return completion.choices[0].message.content


def make_clues():
    clues = list()
    for word in tqdm.tqdm(read_words(_WORDS_PATH)):
        clue = make_clue(word)
        time.sleep(_TIMEOUT)
        clues.append(word + " " + clue.strip('"') + "\n")

    with open(_WORDS_WITH_CLUES, "w") as f:
        f.writelines(clues)


if __name__ == "__main__":
    make_clues()
