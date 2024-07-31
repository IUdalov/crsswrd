import json
import shutil
from dataclasses import dataclass
import os

# code taken from

_DATA_DIR = os.path.join(os.path.abspath(""), "../../data")
_RESULT_DIR = os.path.join(os.path.abspath(""), "../../data/res")

_WEB_PAGE_DIR = os.path.join(os.path.abspath(""), "..")

_TIMEOUT_SEC = 20

@dataclass
class Placement:
    i: int
    j: int
    word: str
    is_horizontal: bool
    position: int = 0


def read_field(path):
    with open(path, 'r') as f:
        return [line.strip("\n") for line in f.readlines()]


def find_placements(field):
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
                    placements.append(Placement(i=i, j=jStart, word=buf, is_horizontal=True))
                buf = ""

        if len(buf) > 1:
            placements.append(Placement(i=i, j=jStart, word=buf, is_horizontal=True))

    for j in range(len(field[0])):
        buf = ""
        iStart = 0
        for i in range(len(field[0])):
            letter = field[i][j]
            if 'a' <= letter <= 'z':
                if len(buf) == 0:
                    iStart = i
                buf += letter
            else:
                if len(buf) > 1:
                    placements.append(Placement(i=iStart, j=j, word=buf, is_horizontal=False))
                buf = ""
    return placements

def read_clues():
    clues = dict()
    # with open(_DATA_DIR + "ml_words_with_clues.txt") as f:
    #     f.readlines()
    #
    #
    return clues


def format_placement(p):
    return {
        "clue": "First letter of greek alphabet",
        "answer": p.word,
        "position": p.position,
        "orientation": "across" if p.is_horizontal else "down",
        "startx": p.i ,
        "starty": p.j
    }


def make_web_page(placements, clues):
    if os.path.exists(_RESULT_DIR):
        shutil.rmtree(_RESULT_DIR)
    os.makedirs(_RESULT_DIR + "/js")
    shutil.copy(_WEB_PAGE_DIR + "/index.html", _RESULT_DIR)
    shutil.copy(_WEB_PAGE_DIR + "/js/jquery.crossword.js", _RESULT_DIR + "/js")

    formated_json = json.dumps([format_placement(p) for p in placements])
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
    last_solution = list(sorted(filter(lambda x: ".tpl." in x, os.listdir(_DATA_DIR))))[-1]
    solution_path = os.path.join(_DATA_DIR, last_solution)
    print(solution_path)
    field = read_field(solution_path)
    placements = find_placements(field)
    across = 1
    down = 1
    for p in placements:
        if p.is_horizontal:
            p.position = across
            across += 1
        else:
            p.position = down
            across += 1

    for i, p in enumerate(placements):
        print(f"{i}: {p}")

    clues = read_clues()
    make_web_page(placements, clues)



if __name__ == "__main__":
    make_crossword()