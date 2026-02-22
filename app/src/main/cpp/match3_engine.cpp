//
// Created by Nguyễn Tuấn Anh on 15/2/26.
//
#include "match3_engine.h"
#include <random>
#include <iostream>
#include <android/log.h>
#define LOG_TAG "Match3Engine" // Replace "MyAppTag" with your desired tag
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

Match3Engine::Match3Engine(int width, int height, int itemTypes):
    width(width), height(height), itemTypes(itemTypes) {
    grid.resize(height, vector<Cell>(width));

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, itemTypes - 1);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            grid[row][col].type = dis(gen);
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

SpecialType Match3Engine::getSpecialType(int row, int col) {
    if (!isInBounds(row, col)) {
        return SpecialType::NONE;
    }
    return grid[row][col].specialType;
}

int Match3Engine::countConsecutive(int row, int col, int dRow, int dCol, int itemType) {
    int count = 0;
    int nRow = row + dRow;
    int nCol = col + dCol;

    while (isInBounds(nRow, nCol) && grid[nRow][nCol].type == itemType) {
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
    if (itemType == EMPTY_CELL) {
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
        case MatchPattern::MATCH_3:
            if (horizontal >= 3) {
                for (int i = col - left; i <= col + right; i++) {
                    result.cells.insert({row, i});
                }
            }
            break;
        case MatchPattern::MATCH_4_VERTICAL:
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
    SpecialType specialType = SpecialType::NONE;
    switch (match.pattern) {
        case MatchPattern::MATCH_4_HORIZONTAL:
            specialType = SpecialType::STRIPED_HORIZONTAL;
            break;
        case MatchPattern::MATCH_4_VERTICAL:
            specialType = SpecialType::STRIPED_VERTICAL;
            break;
        case MatchPattern::MATCH_5:
            specialType = SpecialType::COLOR_BOMB;
            break;
        case MatchPattern::MATCH_L:
        case MatchPattern::MATCH_T:
            specialType = SpecialType::WRAPPED;
            break;
        default:
            break;
    }
    grid[erow][ecol].type = match.itemType;
    grid[erow][ecol].specialType = specialType;
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

    return allMatches;
}

int Match3Engine::processCascadeWithSpecials() {
    int cascadeCount = 0;
    const int MAX_CASCADES = 100;

    while (cascadeCount < MAX_CASCADES) {
        auto matches = findAllMatchesWithPatterns();
        if (matches.empty()) {
            break;
        }
        cascadeCount++;

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
                    break;
            }

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

        applyGravity();
        refillSmart();
    }

    return cascadeCount;
}

void Match3Engine::setGrid(vector<vector<Cell>> grid)  {
    this->grid = grid;
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

void Match3Engine::applyGravity() {
    // Process mỗi column độc lập
    for (int col = 0; col < width; ++col) {
        int writePos = height - 1;  // Start from bottom

        // Scan từ dưới lên, collect non-empty items
        for (int row = height - 1; row >= 0; --row) {
            if (grid[row][col].type != EMPTY_CELL) {
                // Move item to writePos
                if (row != writePos) {
                    grid[writePos][col] = grid[row][col];
                    grid[row][col] = EMPTY_CELL;
                }
                writePos--;  // Next write position moves up
            }
        }
    }
}

void Match3Engine::refillSmart() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, itemTypes - 1);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (grid[row][col].type == EMPTY_CELL) {
                int newItem;
                int attempts = 0;

                do {
                    newItem = dis(gen);
                    attempts++;

                    if (attempts >= MAX_ATTEMPTS) {
                        cerr << "Warning: Forced to create match at ("
                             << row << "," << col << ")\n";
                        break;
                    }
                }
                while (wouldCreateMatch(row, col, newItem));
                grid[row][col] = newItem;
            }
        }
    }
}

bool Match3Engine::wouldCreateMatch(int row, int col, int itemType) {
    int originalItem = grid[row][col].type;
    grid[row][col] = itemType;

    bool hasHorizontalMatch = hasHorizontalMatchAt(row, col);
    bool hasVerticalMatch = hasVerticalMatchAt(row, col);

    grid[row][col] = originalItem;

    return hasHorizontalMatch || hasVerticalMatch;
}

bool Match3Engine::hasHorizontalMatchAt(int row, int col) {
    int itemType = grid[row][col].type;

    int leftCount = 0;
    for (int i = col - 1; i >= 0 && grid[row][i].type == itemType; i--) {
        leftCount++;
    }

    int rightCount = 0;
    for (int i = col + 1; i < width && grid[row][i].type == itemType; i++) {
        rightCount++;
    }

    int totalMatch = leftCount + 1 + rightCount;

    return totalMatch >= 3;
}

bool Match3Engine::hasVerticalMatchAt(int row, int col) {
    int itemType = grid[row][col].type;

    int topCount = 0;
    for (int i = row - 1; i >= 0 && grid[i][col].type == itemType; i--) {
        topCount++;
    }

    int bottomCount = 0;
    for (int i = row + 1; i < height && grid[i][col].type == itemType; i++) {
        bottomCount++;
    }

    int totalMatch = topCount + 1 + bottomCount;

    return totalMatch >= 3;
}

void Match3Engine::refillFromTop() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, itemTypes);

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
                newItem = dis(gen);
                attempts++;

                if (attempts >= MAX_ATTEMPTS) {
                    break;
                }
            }
            while (wouldCreateMatch(row, col, newItem));
            grid[row][col] = newItem;
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
        grid[row][col] = EMPTY_CELL;
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
    auto matches = findAllMatches();
    if (matches.empty()) {
        std::swap(grid[row1][col1], grid[row2][col2]);
        return false;
    }

    processCascade();

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
            if (grid[row][col].type != EMPTY_CELL) {
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
            if (grid[row][col].type != EMPTY_CELL) {
                grid[row][col] = items[idx++];
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
    ::swap(grid[row1][col1], grid[row2][col2]);
    bool hasMatch = checkMatchAt(row1, col1) || checkMatchAt(row2, col2);
    ::swap(grid[row1][col1], grid[row2][col2]);
    return hasMatch;
}

bool Match3Engine::checkMatchAt(int row, int col) {
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