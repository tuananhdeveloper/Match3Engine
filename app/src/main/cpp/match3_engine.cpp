//
// Created by Nguyễn Tuấn Anh on 15/2/26.
//
#include "match3_engine.h"
#include <random>
#include <iostream>
#define LOG_TAG "Match3Engine"
#ifdef __ANDROID__
#include <android/log.h>
    #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#include <iostream>
#define LOGD(...) printf(__VA_ARGS__); printf("\n")
#endif

Match3Engine::Match3Engine(int width, int height, vector<int> itemTypes):
    width(width), height(height), itemTypes(itemTypes) {
    grid.resize(height, vector<Cell>(width));

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, itemTypes.size() - 1);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            grid[row][col].type = itemTypes[dis(gen)];
            grid[row][col].specialType = SpecialType::NONE;
        }
    }
}

int Match3Engine::getItem(int col, int row) {
    if (!isInBounds(row, col)) {
        return -1;
    }
    return grid[row][col].type;
}

int Match3Engine::getSpecialType(int row, int col) {
    if (!isInBounds(row, col)) {
        return SpecialType::NONE;
    }
    return grid[row][col].specialType;
}

int Match3Engine::countConsecutive(int row, int col, int dRow, int dCol, int itemType) {
    int count = 0;
    int nRow = row + dRow;
    int nCol = col + dCol;

    while (isInBounds(nRow, nCol) && grid[nRow][nCol].type == itemType
        && !isSpecialType(grid[nRow][nCol].specialType)) {
        count++;
        nRow += dRow;
        nCol += dCol;
    }

    return count;
}

MatchResult Match3Engine::detectPatternAt(int row, int col) {
    MatchResult result;
    result.pattern = MatchPattern::NONE;
    result.epicenter = {-1, -1};

    int itemType = grid[row][col].type;
    if (itemType == EMPTY_CELL || itemType == holeItemId) {
        return result;
    }

    int left = countConsecutive(row, col, 0, -1, itemType);
    int right = countConsecutive(row, col, 0, 1, itemType);
    int up = countConsecutive(row, col, -1, 0, itemType);
    int down = countConsecutive(row, col, 1, 0, itemType);

    int horizontal = left + 1 + right;
    int vertical = up + 1 + down;

    result.pattern = analyzeMatchPattern(row, col, left, right, up, down);
    result.itemType = itemType;
    result.epicenter = {row, col};

    switch (result.pattern) {
        case MatchPattern::MATCH_5:
        case MatchPattern::MATCH_4_HORIZONTAL:
        case MatchPattern::MATCH_4_VERTICAL:
        case MatchPattern::MATCH_3:
            if (horizontal >= 3) {
                for (int i = col - left; i <= col + right; i++) {
                    result.cells.insert({row, i});
                }
            }
            if (vertical >= 3) {
                for (int i = row - up; i <= row + down; i++) {
                    result.cells.insert({i, col});
                }
            }
            break;
        case MatchPattern::MATCH_L:
        case MatchPattern::MATCH_T:
            for (int i = col - left; i <= col + right; i++) {
                result.cells.insert({row, i});
            }
            for (int i = row - up; i <= row + down; i++) {
                result.cells.insert({i, col});
            }
            break;
        default:
            break;
    }

    bool isFromUser = false;
    for (pair<int, int> item: result.cells) {
        if (selectedRow == item.first && selectedCol == item.second) {
            isFromUser = true;
            break;
        }
    }

    if (isFromUser) {
        result.epicenter = {selectedRow, selectedCol};
        selectedRow = -1;
        selectedCol = -1;
    }
    else {
        result.epicenter = {row, col};
    }

    return result;
}

MatchPattern Match3Engine::analyzeMatchPattern(int row, int col, int left, int right, int up,int down) {
    int horizontal = left + 1 + right;
    int vertical = up + 1 + down;

    //Priority:
    //First: Match 5
    if (horizontal >= 5 || vertical >= 5) {
        return MatchPattern::MATCH_5;
    }
    //Second: Match T, L
    if (horizontal >= 3 && vertical >= 3) {
        if (isTPattern(row, col, left, right, up, down)) {
            return MatchPattern::MATCH_T;
        }
        if (isLPattern(row, col, left, right, up, down)) {
            return MatchPattern::MATCH_L;
        }
    }
    //Third: Match 4
    if (horizontal == 4) {
        return MatchPattern::MATCH_4_HORIZONTAL;
    }
    if (vertical == 4) {
        return MatchPattern::MATCH_4_VERTICAL;
    }
    //Forth: Match 3
    if (horizontal >= 3 || vertical >= 3) {
        return MatchPattern::MATCH_3;
    }

    return MatchPattern::NONE;
}

bool Match3Engine::isLPattern(int row, int col, int left, int right, int up, int down) {
    if (left >= 2 && down >= 2) {
        return true;
    }
    if (right >= 2 && down >= 2) {
        return true;
    }
    if (left >= 2 && up >= 2) {
        return true;
    }
    if (right >= 2 && up >= 2) {
        return true;
    }
    return false;
}

bool Match3Engine::isTPattern(int row, int col, int left, int right, int up, int down) {
    if (left >= 1 && right >= 1 && up >= 2) {
        return true;
    }
    if (left >= 1 && right >= 1 && down >= 2) {
        return true;
    }
    if (up >= 1 && down >= 1 && left >= 2) {
        return true;
    }
    if (up >= 1 && down >= 1 && right >= 2) {
        return true;
    }
    return false;
}

void Match3Engine::spawnSpecialCell(const MatchResult &match) {
    if (match.pattern == MatchPattern::NONE || match.pattern == MatchPattern::MATCH_3) {
        return;
    }

    int erow = match.epicenter.first;
    int ecol = match.epicenter.second;

    if (!isInBounds(erow, ecol)) {
        return;
    }
    int type = SpecialType::NONE;
    switch (match.pattern) {
        case MatchPattern::MATCH_4_HORIZONTAL:
            type = SpecialType::STRIPED_HORIZONTAL;
            break;
        case MatchPattern::MATCH_4_VERTICAL:
            type = SpecialType::STRIPED_VERTICAL;
            break;
        case MatchPattern::MATCH_5:
            type = SpecialType::COLOR_BOMB;
            break;
        case MatchPattern::MATCH_L:
        case MatchPattern::MATCH_T:
            type = SpecialType::WRAPPED;
            break;
        default:
            break;
    }
    if (type != SpecialType::NONE) {
        LOGD("TYPE: %d", specialIndexMap[{match.itemType, type}]);
        grid[erow][ecol].type = specialIndexMap[{match.itemType, type}];
    }
    else {
        grid[erow][ecol].type = match.itemType;
    }
    grid[erow][ecol].specialType = type;
}

vector<MatchResult> Match3Engine::findAllMatchesWithPatterns() {
    vector<MatchResult> allMatches;
    set<pair<int, int>> processedCells;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (processedCells.count({row, col})) {
                continue;
            }
            MatchResult match = detectPatternAt(row, col);
            if (match.pattern != MatchPattern::NONE) {
                allMatches.push_back(match);

                for (const auto& cell: match.cells) {
                    processedCells.insert(cell);
                }
            }
        }
    }

    if (comboResult.has_value()) {
        allMatches.push_back(comboResult.value());
    }

    return allMatches;
}

void Match3Engine::applyGravityAndRefillStream(EventWriter* writer, int cascade) {
    for (int col = 0; col < width; col++) {
        int fallingRow = 0;
        for (int destRow = height - 1; destRow >= 0; --destRow) {
            if (grid[destRow][col].type != EMPTY_CELL) {
                continue;
            }
            int srcRow = destRow - 1;
            while (srcRow >= 0 && grid[srcRow][col].type == EMPTY_CELL) {
                srcRow--;
            }
            if (srcRow >= 0) {
                Cell cell = grid[srcRow][col];
                grid[destRow][col] = cell;
                grid[srcRow][col] = Cell(EMPTY_CELL);
                if (writer) {
                    writer->push8(static_cast<int>(EventType::FALL), srcRow, col, destRow, col,
                                  cell.type, static_cast<int>(cell.specialType), cascade);
                }
            }
            else {
                int newItem;
                int attempts = 0;
                do {
                    newItem = spawnNewItem();
                    attempts++;

                    if (attempts >= MAX_ATTEMPTS) {
                        break;
                    }
                }
                while (wouldCreateMatch(destRow, col, newItem));
                grid[destRow][col].type = newItem;
                grid[destRow][col].specialType = getSpecialTypeFromIndex(newItem);
                if (writer) {
                    writer->push8(static_cast<int>(EventType::SPAWN), --fallingRow, col, destRow, col,
                                  grid[destRow][col].type, static_cast<int>(grid[destRow][col].specialType), cascade);
                }
            }
        }
    }
}

int Match3Engine::processCascadeWithSpecials(bool streaming, bool isRefillingSmart, EventWriter* writer) {
    int cascadeCount = 0;
    const int MAX_CASCADES = 100;

    while (cascadeCount < MAX_CASCADES) {
        auto matches = findAllMatchesWithPatterns();
        if (matches.empty()) {
            break;
        }
        cascadeCount++;
        for (const auto& result: matches) {
            LOGD("CASCADE COUNT: %d", cascadeCount);
            LOGD("TOTAL CELLS: %d", result.cells.size());
            updateScore(result.cells.size(), false,
                        cascadeCount);
            for (auto cell: result.cells) {
                removedCells.push_back(grid[cell.first][cell.second]);
            }
        }

        for (const auto& match: matches) {
            switch (match.pattern) {
                case MatchPattern::MATCH_3:
                    LOGD("MATCH - 3");
                    break;
                case MatchPattern::MATCH_4_VERTICAL:
                    LOGD("MATCH - 4 (V)");
                    break;
                case MatchPattern::MATCH_4_HORIZONTAL:
                    LOGD("MATCH - 4 (H)");
                    break;
                case MatchPattern::MATCH_5:
                    LOGD("MATCH - 5");
                    break;
                case MatchPattern::MATCH_T:
                    LOGD("MATCH - T");
                    break;
                case MatchPattern::MATCH_L:
                    LOGD("MATCH - L");
                    break;
                default:
                    LOGD("COMBO");
                    break;
            }

            if (comboResult.has_value() && match.pattern == comboResult->pattern) {
                for (const auto& cell: comboResult->cells) {
                    grid[cell.first][cell.second].type = EMPTY_CELL;
                    grid[cell.first][cell.second].specialType = SpecialType::NONE;
                }
                comboResult = nullopt;
            }
            else {
                for (const auto& cell: match.cells) {
                    if (cell.first != match.epicenter.first || cell.second != match.epicenter.second) {
                        grid[cell.first][cell.second].type = EMPTY_CELL;
                        grid[cell.first][cell.second].specialType = SpecialType::NONE;
                    }
                }

                spawnSpecialCell(match);

                if (match.pattern == MatchPattern::MATCH_3) {
                    grid[match.epicenter.first][match.epicenter.second].type = EMPTY_CELL;
                }
            }
        }

        if (holeItemId > 0) {
            applyGravity(writer);
            refillSmart(writer);
        }
        else if (!streaming) {
            applyGravity(writer);
            if (isRefillingSmart) {
                refillSmart(writer);
            }
            else {
                refillFromTop(writer);
            }
        }
        else {
            applyGravityAndRefillStream(writer, cascadeCount);
        }
    }

    return cascadeCount;
}

void Match3Engine::setGrid(vector<vector<Cell>> grid)  {
    this->grid = grid;
    this->originalGrid = grid;
}

set<pair<int, int>> Match3Engine::findHorizontalMatches(int row) {
    set<pair<int, int>> matches;

    if (width < 3) {
        return matches;
    }

    int currentType = grid[row][0].type;
    int matchStart = 0;
    int matchLength = 1;

    for (int col = 1; col < width; ++col) {
        if (isSpecialType(grid[row][col].specialType)) {
            continue;
        }
        if (grid[row][col].type == currentType && currentType != EMPTY_CELL) {
            matchLength++;
        }
        else {
            if (matchLength >= 3) {
                for (int col = matchStart; col < matchStart + matchLength; ++col) {
                    matches.insert({row, col});
                }
            }

            currentType = grid[row][col].type;
            matchStart = col;
            matchLength = 1;
        }
    }

    if (matchLength >= 3) {
        for (int col = matchStart; col < matchStart + matchLength; ++col) {
            matches.insert({row, col});
        }
    }

    return matches;
}

set<pair<int, int>> Match3Engine::findVerticalMatches(int col) {
    set<pair<int, int>> matches;

    if (height < 3) {
        return matches;
    }

    int currentType = grid[0][col].type;
    int matchStart = 0;
    int matchLength = 1;

    for (int row = 1; row < width; ++row) {
        if (isSpecialType(grid[row][col].specialType)) {
            continue;
        }
        if (grid[row][col].type == currentType && currentType != EMPTY_CELL) {
            matchLength++;
        }
        else {
            if (matchLength >= 3) {
                for (int row = matchStart; row < matchStart + matchLength; row++) {
                    matches.insert({row, col});
                }
            }

            currentType = grid[row][col].type;
            matchStart = row;
            matchLength = 1;
        }
    }

    if (matchLength >= 3) {
        for (int row = matchStart; row < matchStart + matchLength; row++) {
            matches.insert({row, col});
        }
    }

    return matches;
}

set<pair<int, int>> Match3Engine::findAllMatches() {
    set<pair<int, int>> allMatches;

    for (int row = 0; row < height; row++) {
        auto matches = findHorizontalMatches(row);
        allMatches.insert(matches.begin(), matches.end());
    }
    for (int col = 0; col < width; col++) {
        auto matches = findVerticalMatches(col);
        allMatches.insert(matches.begin(), matches.end());
    }

    return allMatches;
}

void Match3Engine::applyGravity(EventWriter* writer) {
    // Process mỗi column độc lập
    for (int col = 0; col < width; ++col) {
        int writePos = height - 1;  // Start from bottom

        // Scan từ dưới lên, collect non-empty items
        for (int row = height - 1; row >= 0; --row) {
            if (grid[row][col].type != EMPTY_CELL) {
                // Move item to writePos
                if (row != writePos && grid[row][col].type != holeItemId
                        && grid[writePos][col].type != holeItemId) {
                    grid[writePos][col] = grid[row][col];
                    grid[row][col].type = EMPTY_CELL;
                    if (writer) {
                        writer->push8(static_cast<int>(EventType::FALL), row, col, writePos, col,
                                     grid[writePos][col].type, static_cast<int>(grid[writePos][col].specialType), 0);
                    }
                }
                writePos--;  // Next write position moves up
            }
        }
    }
}

void Match3Engine::refillSmart(EventWriter* writer) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, itemTypes.size() - 1);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (grid[row][col].type == EMPTY_CELL) {
                int newItem;
                int attempts = 0;

                do {
                    newItem = itemTypes[dis(gen)];
                    attempts++;

                    if (attempts >= MAX_ATTEMPTS) {
                        cerr << "Warning: Forced to create match at ("
                             << row << "," << col << ")\n";
                        break;
                    }
                }
                while (newItem == holeItemId || wouldCreateMatch(row, col, newItem));
                grid[row][col].type = newItem;
                grid[row][col].specialType = getSpecialTypeFromIndex(newItem);
                if (writer) {
                    writer->push8(static_cast<int>(EventType::SPAWN), row, col, row, col,
                                 grid[row][col].type, static_cast<int>(grid[row][col].specialType), 0);
                }
            }
        }
    }
}

bool Match3Engine::wouldCreateMatch(int row, int col, int itemType) {
    int originalItem = grid[row][col].type;
    grid[row][col].type = itemType;

    bool hasHorizontalMatch = hasHorizontalMatchAt(row, col);
    bool hasVerticalMatch = hasVerticalMatchAt(row, col);

    grid[row][col].type = originalItem;

    return hasHorizontalMatch || hasVerticalMatch;
}

bool Match3Engine::hasHorizontalMatchAt(int row, int col) {
    int itemType = grid[row][col].type;

    int leftCount = 0;
    for (int i = col - 1; i >= 0 && grid[row][i].type == itemType
        && !isSpecialType(grid[row][i].specialType); i--) {
        leftCount++;
    }

    int rightCount = 0;
    for (int i = col + 1; i < width && grid[row][i].type == itemType
        && !isSpecialType(grid[row][i].specialType); i++) {
        rightCount++;
    }

    int totalMatch = leftCount + 1 + rightCount;

    return totalMatch >= 3;
}

bool Match3Engine::hasVerticalMatchAt(int row, int col) {
    int itemType = grid[row][col].type;

    int topCount = 0;
    for (int i = row - 1; i >= 0 && grid[i][col].type == itemType
        && !isSpecialType(grid[i][col].specialType); i--) {
        topCount++;
    }

    int bottomCount = 0;
    for (int i = row + 1; i < height && grid[i][col].type == itemType
        && !isSpecialType(grid[i][col].specialType); i++) {
        bottomCount++;
    }

    int totalMatch = topCount + 1 + bottomCount;

    return totalMatch >= 3;
}

void Match3Engine::refillFromTop(EventWriter* writer) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, itemTypes.size() - 1);

    for (int col = 0; col < width; col++) {
        int emptyCount = 0;

        for (int row = 0; row < height; row++) {
            if (grid[row][col].type == EMPTY_CELL) {
                emptyCount++;
            }
        }

        for (int row = 0; row < emptyCount; row++) {
            int newItem;
            int attempts = 0;
            do {
                newItem = itemTypes[dis(gen)];
                attempts++;

                if (attempts >= MAX_ATTEMPTS) {
                    break;
                }
            }
            while (wouldCreateMatch(row, col, newItem));
            grid[row][col].type = newItem;
            if (writer) {
                writer->push8(static_cast<int>(EventType::SPAWN), -1, col, row, col,
                             grid[row][col].type, static_cast<int>(grid[row][col].specialType), 0);
            }
        }
    }
}

int Match3Engine::processCascade() {
    int cascadeCount = 0;

    while (true) {
        auto matches = findAllMatches();
        if (matches.empty()) {
            break;
        }
        cascadeCount++;
        removeMatches(matches);
        applyGravity();
        refillFromTop();
    }

    return cascadeCount;
}

void Match3Engine::removeMatches(const set<pair<int, int>> &matches) {
    for (const auto& [row, col]: matches) {
        grid[row][col].type = EMPTY_CELL;
    }
}

bool Match3Engine::swap(int row1, int col1, int row2, int col2) {
    if (!isInBounds(row1, col1) || !isInBounds(row1, col1)) {
        return false;
    }
    if (!isAdjacent(row1, col1, row2, col2)) {
        return false;
    }
    std::swap(grid[row1][col1], grid[row2][col2]);

    comboResult = getCombo(row1, col1, row2, col2);
    if (comboResult.has_value() && comboResult->pattern != MatchPattern::NONE) {
        return true;
    }

    auto matches = findAllMatches();
    if (matches.empty()) {
        std::swap(grid[row1][col1], grid[row2][col2]);
        return false;
    }
    selectedRow = row2;
    selectedCol = col2;
    return true;
}

bool Match3Engine::isAdjacent(int row1, int col1, int row2, int col2) {
    int dx = abs(col1 - col2);
    int dy = abs(row1 - row2);

    return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}

bool Match3Engine::isInBounds(int row, int col) {
    return row >= 0 && row < height && col >= 0 && col < width;
}

bool Match3Engine::hasValidMoves() {
    // Đổi chỗ mọi ô
    // Kiểm tra có tồn tại match không
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (col < width - 1) {
                if (wouldCreateMatchAfterSwap(row, col, row, col + 1)) {
                    LOGD("Valid move: (%d, %d), (%d, %d)", row, col, row, col + 1);
                    return true;
                }
            }
            if (row < height - 1) {
                if (wouldCreateMatchAfterSwap(row, col, row + 1, col)) {
                    LOGD("Valid move: (%d, %d), (%d, %d)", row, col, row + 1, col);
                    return true;
                }
            }
        }
    }

    return false;
}

void Match3Engine::shuffle() {
    LOGD("Shuffling board...\n");
    vector<int> items;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (grid[row][col].type != EMPTY_CELL && grid[row][col].type != holeItemId) {
                items.push_back(grid[row][col].type);
            }
        }
    }
    random_device rd;
    mt19937 gen(rd());
    for (int i = items.size() - 1; i > 0; --i) {
        uniform_int_distribution<> dis(0, i);
        int j = dis(gen);
        ::swap(items[i], items[j]);
    }

    int idx = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (grid[row][col].type != EMPTY_CELL && grid[row][col].type != holeItemId) {
                grid[row][col].type = items[idx++];
            }
        }
    }

    if (!hasValidMoves()) {
        LOGD("Shuffle didn't create moves, shuffling again...\n");
        shuffle();
    }
}

int Match3Engine::countValidMoves() {
    int count = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (col < width - 1) {
                if (wouldCreateMatchAfterSwap(row, col, row, col + 1)) {
                    LOGD("Valid move: (%d, %d), (%d, %d)", row, col, row, col + 1);
                    count++;
                }
            }
            if (row < height - 1) {
                if (wouldCreateMatchAfterSwap(row, col, row + 1, col)) {
                    LOGD("Valid move: (%d, %d), (%d, %d)", row, col, row + 1, col);
                    count++;
                }
            }
        }
    }

    return count;
}

bool Match3Engine::wouldCreateMatchAfterSwap(int row1, int col1, int row2, int col2) {
    if (isSpecialType(grid[row1][col1].specialType)
        || isSpecialType(grid[row2][col2].specialType)) {
        return false;
    }
    ::swap(grid[row1][col1], grid[row2][col2]);
    bool hasMatch = checkMatchAt(row1, col1) || checkMatchAt(row2, col2);
    ::swap(grid[row1][col1], grid[row2][col2]);
    return hasMatch;
}

bool Match3Engine::checkMatchAt(int row, int col) {
    if (grid[row][col].type == holeItemId) {
        return false;
    }
    return hasHorizontalMatchAt(row, col) || hasVerticalMatchAt(row, col);
}

optional<Move> Match3Engine::findHint() {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (col < width - 1) {
                if (wouldCreateMatchAfterSwap(row, col, row, col + 1)) {
                    return Move{row, col, row, col + 1};
                }
            }
            if (row < height - 1) {
                if (wouldCreateMatchAfterSwap(row, col, row + 1, col)) {
                    return Move{row, col, row + 1, col};
                }
            }
        }
    }

    return nullopt;
}

void Match3Engine::reset() {
    this->grid = originalGrid;
    this->totalScore = 0;
}

void Match3Engine::setHoleItemId(int id) {
    this->holeItemId = id;
}

const char* getComboPatternName(MatchPattern pattern) {
    switch (pattern) {
        case MatchPattern::STRIPED_STRIPED:   return "STRIPED_STRIPED";
        case MatchPattern::STRIPED_WRAPPED: return "STRIPED_WRAPPED";
        case MatchPattern::WRAPPED_WRAPPED:  return "WRAPPED_WRAPPED";
        case MatchPattern::COLOR_BOMB_WRAPPED:  return "COLOR_BOMB_WRAPPED";
        case MatchPattern::COLOR_BOMB_STRIPED:  return "COLOR_BOMB_STRIPED";
        case MatchPattern::COLOR_BOMB_COLOR_BOMB:  return "COLOR_BOMB_COLOR_BOMB";
        default: return "Unknown";
    }
}

optional<MatchResult> Match3Engine::getCombo(int row1, int col1, int row2, int col2) {
    if (findComboPattern(row1, col1, row2, col2) != MatchPattern::NONE) {
        LOGD("COMBO: %s", getComboPatternName(findComboPattern(row1, col1, row2, col2)));
        return getComboMatchResult(row1, col1, row2, col2);
    }
    LOGD("NO COMBO");
    return nullopt;
}

MatchResult Match3Engine::getComboMatchResult(int row1, int col1, int row2, int col2) {
    Cell firstItem = grid[row1][col1];
    Cell secondItem = grid[row2][col2];
    MatchResult result;
    result.epicenter = {row1, col1};

    result.pattern = findComboPattern(row1, col1, row2, col2);
    switch (result.pattern) {
        case MatchPattern::STRIPED_STRIPED:
            for (int row = 0; row < height; row++) {
                result.cells.insert({row, col2});
            }
            for (int col = 0; col < width; col++) {
                result.cells.insert({row2, col});
            }
            break;
        case MatchPattern::STRIPED_WRAPPED:
            if (row1 == row2) { // swap horizontally
                for (int col = 0; col < width; col++) {
                    result.cells.insert({row2, col});
                }
            }
            else if (col1 == col2) { // swap vertically
                for (int row = 0; row < height; row++) {
                    result.cells.insert({row, col2});
                }
            }
            break;
        case MatchPattern::WRAPPED_WRAPPED:
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    if (abs(row - row2) <= 2 && abs(col - col2) <= 2) {
                        result.cells.insert({row, col});
                    }
                }
            }
            break;
        case MatchPattern::COLOR_BOMB_NORMAL_TYPE:
            {
                int type = (firstItem.specialType == SpecialType::NONE) ? firstItem.type : secondItem.type;
                for (int row = 0; row < height; row++) {
                    for (int col = 0; col < width; col++) {
                        if (grid[row][col].type == type || grid[row][col].specialType == SpecialType::COLOR_BOMB) {
                            result.cells.insert({row, col});
                        }
                    }
                }
            }
            break;
        case MatchPattern::COLOR_BOMB_STRIPED:
            {
                int type = (firstItem.specialType == SpecialType::STRIPED_VERTICAL
                            || firstItem.specialType == SpecialType::STRIPED_HORIZONTAL) ? firstItem.type : secondItem.type;
                int specialType = (firstItem.specialType == SpecialType::STRIPED_VERTICAL
                            || firstItem.specialType == SpecialType::STRIPED_HORIZONTAL) ? firstItem.specialType : secondItem.specialType;
                vector<pair<int, int>> filterPositions;
                int transformTotal = 0;

                for (int row = 0; row < height; row++) {
                    for (int col = 0; col < width; col++) {
                        int index = grid[row][col].type;
                        if (specialIndexMap[{index, SpecialType::STRIPED_VERTICAL}] == type
                            || specialIndexMap[{index, SpecialType::STRIPED_HORIZONTAL}] == type
                            && grid[row][col].specialType == SpecialType::NONE) {
                            grid[row][col] = type;
                            grid[row][col].specialType = specialType;
                            transformTotal++;
                        }
                        if (grid[row][col].specialType == specialType) {
                            filterPositions.push_back({row, col});
                        }
                    }
                }
                updateScore(transformTotal, true, 1);
                for (pair<int, int> position: filterPositions) {
                    int filterRow = position.first;
                    int filterCol = position.second;
                    for (int row = 0; row < height; row++) {
                        result.cells.insert({row, filterCol});
                    }
                    for (int col = 0; col < width; col++) {
                        result.cells.insert({filterRow, col});
                    }
                }
            }
            break;
        case MatchPattern::COLOR_BOMB_WRAPPED:
            {
                int type = (firstItem.specialType == SpecialType::WRAPPED) ? firstItem.type : secondItem.type;
                vector<pair<int, int>> wrappedPositions;
                int transformTotal = 0;

                for (int row = 0; row < height; row++) {
                    for (int col = 0; col < width; col++) {
                        int index = grid[row][col].type;
                        if (specialIndexMap[{index, SpecialType::WRAPPED}] == type
                            && grid[row][col].specialType == SpecialType::NONE) {
                            grid[row][col].type = type;
                            grid[row][col].specialType = SpecialType::WRAPPED;
                            transformTotal++;
                        }
                        if(grid[row][col].specialType == SpecialType::WRAPPED) {
                            wrappedPositions.push_back({row, col});
                        }
                    }
                }
                updateScore(transformTotal, true, 1);
                for (pair<int, int> position: wrappedPositions) {
                    for (int row = 0; row < height; row++) {
                        for (int col = 0; col < width; col++) {
                            if (abs(row - position.first) <= 1 && abs(col - position.second) <= 1) {
                                result.cells.insert({row, col});
                            }
                        }
                    }
                }
            }
            break;
        case MatchPattern::COLOR_BOMB_COLOR_BOMB:
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    result.cells.insert({row, col});
                }
            }
            break;
        default:
            break;
    }
    return result;
}

MatchPattern Match3Engine::findComboPattern(int row1, int col1, int row2, int col2) {
    Cell firstItem = grid[row1][col1];
    Cell secondItem = grid[row2][col2];

    if ((firstItem.specialType == SpecialType::STRIPED_VERTICAL && secondItem.specialType == SpecialType::STRIPED_VERTICAL)
        || (firstItem.specialType == SpecialType::STRIPED_HORIZONTAL && secondItem.specialType == SpecialType::STRIPED_HORIZONTAL)
        || (firstItem.specialType == SpecialType::STRIPED_VERTICAL && secondItem.specialType == SpecialType::STRIPED_HORIZONTAL)
        || (firstItem.specialType == SpecialType::STRIPED_HORIZONTAL && secondItem.specialType == SpecialType::STRIPED_VERTICAL)) {
        return MatchPattern::STRIPED_STRIPED;
    }
    if ((firstItem.specialType == SpecialType::STRIPED_VERTICAL && secondItem.specialType == SpecialType::WRAPPED)
        || (firstItem.specialType == SpecialType::STRIPED_HORIZONTAL && secondItem.specialType == SpecialType::WRAPPED)
        || (firstItem.specialType == SpecialType::WRAPPED && secondItem.specialType == SpecialType::STRIPED_HORIZONTAL)
        || (firstItem.specialType == SpecialType::WRAPPED && secondItem.specialType == SpecialType::STRIPED_VERTICAL)) {
        return MatchPattern::STRIPED_WRAPPED;
    }
    if (firstItem.specialType == SpecialType::WRAPPED && secondItem.specialType == SpecialType::WRAPPED) {
        return MatchPattern::WRAPPED_WRAPPED;
    }
    if ((firstItem.specialType == SpecialType::COLOR_BOMB && secondItem.specialType == SpecialType::NONE)
        || (firstItem.specialType == SpecialType::NONE && secondItem.specialType == SpecialType::COLOR_BOMB)) {
        return MatchPattern::COLOR_BOMB_NORMAL_TYPE;
    }
    if ((firstItem.specialType == SpecialType::COLOR_BOMB && secondItem.specialType == SpecialType::STRIPED_VERTICAL)
        || (firstItem.specialType == SpecialType::COLOR_BOMB && secondItem.specialType == SpecialType::STRIPED_HORIZONTAL)
        || (firstItem.specialType == SpecialType::STRIPED_VERTICAL && secondItem.specialType == SpecialType::COLOR_BOMB)
        || (firstItem.specialType == SpecialType::STRIPED_HORIZONTAL && secondItem.specialType == SpecialType::COLOR_BOMB)) {
        return MatchPattern::COLOR_BOMB_STRIPED;
    }
    if ((firstItem.specialType == SpecialType::COLOR_BOMB && secondItem.specialType == SpecialType::WRAPPED)
        || (firstItem.specialType == SpecialType::WRAPPED && secondItem.specialType == SpecialType::COLOR_BOMB)) {
        return MatchPattern::COLOR_BOMB_WRAPPED;
    }
    if (firstItem.specialType == SpecialType::COLOR_BOMB && secondItem.specialType == SpecialType::COLOR_BOMB) {
        return MatchPattern::COLOR_BOMB_COLOR_BOMB;
    }
    return MatchPattern::NONE;
}

bool Match3Engine::isSpecialType(int specialType) const {
    return specialType != SpecialType::NONE;
}

void Match3Engine::setSpecialTypeMap(unordered_map<int, int> map) {
    this->specialTypeMap = map;
    for (int id: itemTypes) {
        if (specialTypeMap[id] == SpecialType::NONE) {
            normalPool.push_back(id);
        }
        else {
            specialPool.push_back(id);
        }
    }
}

SpecialType Match3Engine::getSpecialTypeFromIndex(int index) {
    int specialValue = specialTypeMap[index];
    switch (specialValue) {
        case 0: return STRIPED_VERTICAL;
        case 1: return STRIPED_HORIZONTAL;
        case 2: return COLOR_BOMB;
        case 3: return WRAPPED;
    }
    return NONE;
}

void Match3Engine::setSpecialIndexMap(unordered_map<pair<int, int>, int, pair_hash> map) {
    this->specialIndexMap = map;
}

int Match3Engine::spawnNewItem() {
    int chance = rand() % 100;

    if (chance < 99.5) {
        return normalPool[rand() % normalPool.size()];
    }
    else {
        return specialPool[rand() % specialPool.size()];
    }
}

void Match3Engine::updateBasePoint(int normalBasePoint, int specialBasePoint) {
    this->normalItemBasePoint = normalBasePoint;
    this->specialItemBasePoint = specialBasePoint;
}

int Match3Engine::getTotalScore() {
    return this->totalScore;
}

void Match3Engine::updateScore(int matchCount, bool isSpecial, int comboMultiplier) {
    int baseScore = isSpecial ? specialItemBasePoint : normalItemBasePoint;
    totalScore += matchCount * baseScore * comboMultiplier;
}

vector<Cell> Match3Engine::getRemovedCells() {
    return removedCells;
}