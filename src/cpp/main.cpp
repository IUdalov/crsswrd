#include "kdtree.h"

#include <unordered_set>
#include <iostream>
#include <cstdlib>

constexpr char PLACEHOLDER = '#';
constexpr char BLOCKED_LETTER = '.';
constexpr int MIN_WORD_LEN = 3;

struct Cell {
    char letter = PLACEHOLDER;
    bool horizontal = false;
    bool vertical = false;

    [[nodiscard]] bool filled() const {
        return letter != PLACEHOLDER && letter != BLOCKED_LETTER;
    }
};

constexpr Cell EMPTY_CELL;

struct Slot {
    bool isHorizontal;
    int len;
    int iPos;
    int jPos;
    std::string pattern;
};

using Field = std::vector<std::vector<Cell>>;
double calcDensity(const Field& f) {
    auto area = static_cast<double>(f.size() * f[0].size());
    size_t nLetters = 0;
    for(int i = 0; i < f.size(); i++)
        for (int j = 0; j < f[0].size(); j++)
            if (f[i][j].letter != PLACEHOLDER && f[i][j].letter != BLOCKED_LETTER)
                nLetters++;
    return static_cast<double>(nLetters) / area;
}

void printField(const Field& f, const std::string& tag) {
    auto drawDash = [&f](){
        for(int j = 0; j < f[0].size(); j++)
            std::cout << "+";
        std::cout << std::endl;
    };
    drawDash();
    std::cout << tag << std::endl;
    drawDash();
    for(int i = 0; i < f.size(); i++) {
        for (int j = 0; j < f[0].size(); j++)
            std::cout << (f[i][j].letter == PLACEHOLDER ? ' ' : f[i][j].letter);
        std::cout << std::endl;
    }
    drawDash();
}

Field readField(const std::string& path) {
    std::string buf;
    std::ifstream in(path);
    if (!in.good())
        throw std::runtime_error("Unable to read mask file");

    std::vector<std::string> rows;
    size_t maxLen = 0;
    while (std::getline(in, buf)) {
        rows.push_back(buf);
        std::cout << buf << std::endl;
        maxLen = std::max(maxLen, buf.size());
    }
    maxLen = std::max(maxLen, rows.size());
    Field field(maxLen, std::vector<Cell>(maxLen, Cell{BLOCKED_LETTER, false, false}));
    for(int i = 0; i < rows.size(); i++)
        for(int j = 0; j < rows[i].size(); j++) {
            field[i][j].letter = rows[i][j];
            if (field[i][j].filled())
                field[i][j].horizontal = true;
        }

    return field;
}

void writeField(const std::string& path, const Field& f) {
    std::ofstream out(path);
    std::cout << "Saving " << path << std::endl;

    for(int i = 0; i < f.size(); i++) {
        for(int j = 0; j < f[i].size(); j++)
            out << (f[i][j].filled() ? f[i][j].letter : ' ');
        out << std::endl;
    }
}

std::optional<Slot> randomSlot(const Field& f, bool isHorizontal, bool isFirstPlacement, int maxWordLen) {
    Slot slot;
    slot.isHorizontal = isHorizontal;
    slot.len = rand() % (maxWordLen - MIN_WORD_LEN) + MIN_WORD_LEN;
    slot.iPos = rand() % f.size();
    slot.jPos = rand() % (f.size() - slot.len + 1);
    if (!slot.isHorizontal)
        std::swap(slot.iPos, slot.jPos);

    if (slot.isHorizontal) {
        bool hasVerticalIntersection = false;
        for (int i = 0; i < slot.len; i++) {
            const auto& thisCell = f[slot.iPos][slot.jPos + i];
            const auto& upCell = slot.iPos - 1 >= 0 ? f[slot.iPos - 1][slot.jPos + i] : EMPTY_CELL;
            const auto& downCell = slot.iPos + 1 < f.size() ? f[slot.iPos + 1][slot.jPos + i] : EMPTY_CELL;
            if (thisCell.letter == BLOCKED_LETTER || thisCell.horizontal || upCell.horizontal || downCell.horizontal)
                return std::nullopt;
            hasVerticalIntersection = hasVerticalIntersection || thisCell.vertical;
            if ((upCell.vertical || downCell.vertical) && !thisCell.vertical)
                return std::nullopt;

            slot.pattern.push_back(thisCell.letter);
        }
        if (!hasVerticalIntersection && !isFirstPlacement)
            return std::nullopt;

        const auto& headCell = slot.jPos - 1 >= 0 ? f[slot.iPos][slot.jPos - 1] : EMPTY_CELL;
        const auto& tailCell = slot.jPos + slot.len < f.size() ? f[slot.iPos][slot.jPos + slot.len] : EMPTY_CELL;
        if (headCell.filled() || tailCell.filled())
            return std::nullopt;
    } else {
        bool hasHorizontalIntersection = false;
        for (int i = 0; i < slot.len; i++) {
            const auto& thisCell = f[slot.iPos + i][slot.jPos];
            const auto& leftCell = slot.jPos - 1 >= 0 ? f[slot.iPos + i][slot.jPos - 1] : EMPTY_CELL;
            const auto& rightCell = slot.jPos + 1 < f.size() ? f[slot.iPos + i][slot.jPos + 1] : EMPTY_CELL;

            hasHorizontalIntersection = hasHorizontalIntersection || thisCell.horizontal;

            if (thisCell.letter == BLOCKED_LETTER || thisCell.vertical || leftCell.vertical || rightCell.vertical)
                return std::nullopt;

            if ((leftCell.horizontal || rightCell.horizontal) && !thisCell.horizontal)
                return std::nullopt;
            slot.pattern.push_back(thisCell.letter);
        }

        if (!hasHorizontalIntersection && !isFirstPlacement)
            return std::nullopt;

        const auto& headCell = slot.iPos - 1 >= 0 ? f[slot.iPos - 1][slot.jPos] : EMPTY_CELL;
        const auto& tailCell = slot.iPos + slot.len < f.size() ? f[slot.iPos + slot.len][slot.jPos] : EMPTY_CELL;
        if (headCell.filled() || tailCell.filled())
            return std::nullopt;
    }

    return slot;
}

void makePlacement(Field& f, const Slot& slot, const std::string& word) {
    for(int i = 0; i < slot.len; i++)
        if (slot.isHorizontal) {
            f[slot.iPos][slot.jPos + i].letter = word[i];
            f[slot.iPos][slot.jPos + i].horizontal = true;
        } else {
            f[slot.iPos + i][slot.jPos].letter = word[i];
            f[slot.iPos + i][slot.jPos].vertical = true;
        }
}

int findOneCrossword(const KDDict& kdtree, Field& f) {
    std::unordered_set<std::string> usedWords;

    bool isHorizontal = true;
    for(;;) {
        bool hadSuccessfulPlacement = false;
        for(int times = 4096; times != 0; times--) {
            auto slot = randomSlot(f, isHorizontal, usedWords.empty(), kdtree.maxWordLen());
            if (!slot)
                continue;
            auto matchedWords = kdtree.lookup(slot->pattern);
            if (!matchedWords.empty()) {
                const auto& word = matchedWords[rand() % matchedWords.size()];
                if (usedWords.count(word) != 0) {
                    continue;
                }

                hadSuccessfulPlacement = true;
                makePlacement(f, *slot, word);
                isHorizontal = !isHorizontal;
                usedWords.insert(word);
                break;
            }
        }

        if (!hadSuccessfulPlacement)
            break;
    }
    return usedWords.size();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0]
            << " <path to template/field> <path to dict>"
            << std::endl;
        return 0;
    }

    const auto fieldPath = argv[1];
    const auto wordsPath = argv[2];

    srand(time(nullptr));

    Field plainField = readField(fieldPath);
    KDDict dict(wordsPath);
    std::cout << dict.pretty();

    double maxDensity = -1;
    double nSolutions = 0;
    for(;;) {
        Field field = plainField;
        int wordsUsed = findOneCrossword(dict, field);
        double density = calcDensity(field);
        if (density > maxDensity) {
            maxDensity = density;

            std::stringstream tag;
            tag << "density: " << density << ", words used: " << wordsUsed << "/" << dict.size();
            printField(field, tag.str());

            std::stringstream pathSS;
            pathSS << fieldPath << "." << nSolutions++;
            writeField(pathSS.str(), field);
        }
    }

    return 0;
}