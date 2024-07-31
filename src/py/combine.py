import json
import shutil
from dataclasses import dataclass
import os

# code taken from
_DATA_DIR = os.path.join(os.path.abspath(""), "../../data")
_TRAIT_PATH = os.path.join(_DATA_DIR + "/16x16.tpl.9")
_CLUES_PATH = os.path.join(_DATA_DIR + "/ml_words_with_clues.txt")
_WEB_PAGE_DIR = os.path.join(os.path.abspath(""), "..")
_RESULT_DIR = os.path.join(os.path.abspath(""), "../../data/res")

_TIMEOUT_SEC = 20


def find_placements(path):
    with open(path, 'r') as f:
        field = [line.strip("\n") for line in f.readlines()]

    placements = list()
    for i in range(len(field)):
        buf = ""
        jStart = 0
        for j in range(len(field[0])):
            letter = field[i][j]
            if 'a' <= letter <= 'z':
                if len(buf) == 0:
                    jStart = j
                buf += letter
            else:
                if len(buf) > 1:
                    placements.append({"answer": buf, "orientation": "across", "startx": jStart + 1, "starty": i + 1})
                buf = ""

        if len(buf) > 1:
            placements.append({"answer": buf, "orientation": "across", "startx": jStart + 1, "starty": i + 1})

    for j in range(len(field[0])):
        buf = ""
        iStart = 0
        for i in range(len(field)):
            letter = field[i][j]
            if 'a' <= letter <= 'z':
                if len(buf) == 0:
                    iStart = i
                buf += letter
            else:
                if len(buf) > 1:
                    placements.append({"answer": buf, "orientation": "down", "startx": j + 1, "starty": iStart + 1})
                buf = ""

        if len(buf) > 1:
            placements.append({"answer": buf, "orientation": "down", "startx": j + 1, "starty": iStart + 1})

    return placements


def read_clues(path):
    clues = dict()
    with open(path) as f:
        lines = f.readlines()

    for line in lines:
        tokens = line.strip().split(" ")
        clues[tokens[0]] = " ".join(tokens[1:])

    return clues


def make_html_page(placements):
    if os.path.exists(_RESULT_DIR):
        shutil.rmtree(_RESULT_DIR)
    os.makedirs(_RESULT_DIR + "/js")
    shutil.copy(_WEB_PAGE_DIR + "/index.html", _RESULT_DIR)
    shutil.copy(_WEB_PAGE_DIR + "/js/jquery.crossword.js", _RESULT_DIR + "/js")

    formated_json = json.dumps(placements)
    with open(_RESULT_DIR + "/js/script.js", 'w') as f:
        f.write(f""" 
(function($) {{
	$(function() {{
		var puzzleData = {formated_json}
		$('#puzzle-wrapper').crossword(puzzleData);
	}})
	
}})(jQuery)
        """)


def make_crossword():
    placements = find_placements(_TRAIT_PATH)
    clues = read_clues(_CLUES_PATH)

    placements = sorted(placements, key=lambda p: p["starty"] + p["startx"])
    start_to_pos = dict()
    curr_pos = 1
    for p in placements:
        idx = p["startx"] * 1024 + p["starty"]
        if idx in start_to_pos:
            p["position"] = start_to_pos[idx]
        else:
            p["position"] = curr_pos
            start_to_pos[idx] = curr_pos
            curr_pos += 1

        if p["answer"] not in clues:
            print(f"ERROR: {p['answer']} not in clues")
        p["clue"] = clues[p["answer"]]

    placements = sorted(placements, key=lambda p: p["position"])
    for i, p in enumerate(placements):
        print(f"{i}: {p}")

    make_html_page(placements)



if __name__ == "__main__":
    make_crossword()