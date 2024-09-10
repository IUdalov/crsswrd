# 🧩CRSSWRD - simple crossword generator using ChatGPT and Mote-Carlo
This repo has code to generate crossword puzzle about machine learning.

[**TAKE ME TO THE ML CROSSWORD PUZZLE 🧩**](https://udalovilia.com/crossword/)

## How does it work?
1. [Explanation in Engish]()
2. [Статья на русском](https://habr.com/ru/articles/838674/)

## Reproducing the result
### Install Requirements
1. `pip install --upgrade openai`
1. [CMake](https://cmake.org/) + any `c++17` compatible compiler.
1. Set env variable `OPENAI_API_KEY` to your OpenAI API key.

### Crossword creation
1. `git clone 	git@github.com:IUdalov/crsswrd.git` and install requrements.
1. Decide on set of words to use for crossword and save it to the file. [EXAMPLE](https://github.com/IUdalov/crsswrd/blob/main/data/ml_words.txt)
1. Create or use existing crossword trait. [EXAMPLE](https://github.com/IUdalov/crsswrd/blob/main/data/16x16.tpl)
1. Build cpp program `(mkdir build && cd build && cmake .. && make) `
1. Generate crossword `build/crsswrd data/32x32.tpl data/ml_words.txt`
1. Run `src/py/make_clues.py` with your words to generate clues using ChatGPT.
1. Run `src/py/combine.py` with your clues and crossword - to generate crossword.
1. Open `data/res/index.html`.

### Acknowledgment
1. For crossword puzzle rendering I have used fixed version of code from this repo https://github.com/jweisbeck/Crossword
2. ML words parsed from this repo https://github.com/bfortuner/ml-glossary
