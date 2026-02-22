//
// Created by Nguyễn Tuấn Anh on 15/2/26.
//

#ifndef MATCH3ENGINE_MATCH3_ENGINE_H
#define MATCH3ENGINE_MATCH3_ENGINE_H

#include <set>
#include <utility>
using namespace std;

struct Move {
    int row1, col1, row2, col2;
};

enum class SpecialType {
    NONE,
    STRIPED_HORIZONTAL,
    STRIPED_VERTICAL,
    WRAPPED,
    COLOR_BOMB
};

enum class MatchPattern {
    NONE,
    MATCH_3,
    MATCH_4_HORIZONTAL,
    MATCH_4_VERTICAL,
    MATCH_5,
    MATCH_L,
    MATCH_T
};

struct Cell {
    int type;
    SpecialType specialType;
    Cell() : type(-1), specialType(SpecialType::NONE) {}
    Cell(int t) : type(t), specialType(SpecialType::NONE) {}
};

struct MatchResult {
    MatchPattern pattern;
    set<pair<int, int>> cells;
    pair<int, int> epicenter;
    int itemType;
};

class Match3Engine {
private:
    int width;
    int height;
    int itemTypes;
    vector<vector<Cell>> grid;
    const int EMPTY_CELL = -1;
    const int MAX_ATTEMPTS = 100;

private:
    set<pair<int, int>> findHorizontalMatches(int row);
    set<pair<int, int>> findVerticalMatches(int col);
    void refillSmart();
    void refillFromTop();
    void removeMatches(const set<pair<int, int>>& matches);
    bool wouldCreateMatch(int row, int col, int itemType);
    bool hasVerticalMatchAt(int row, int col);
    bool hasHorizontalMatchAt(int row, int col);
    bool isInBounds(int row, int col);
    bool isAdjacent(int row1, int col1, int row2, int col2);
    bool wouldCreateMatchAfterSwap(int row1, int col1, int row2, int col2);
    bool checkMatchAt(int row, int col);

public:
    Match3Engine(int width, int height, int itemTypes);
    set<pair<int, int>> findAllMatches();
    void setGrid(vector<vector<Cell>> grid);
    int getItem(int col, int row);
    void applyGravity();
    int processCascade();
    bool hasValidMoves();
    void shuffle();
    int countValidMoves();
    optional<Move> findHint();
    MatchResult detectPatternAt(int row, int col);
    MatchPattern analyzeMatchPattern(int row, int col, int left, int right, int up, int down);
    void spawnSpecialCell(const MatchResult& match);
    SpecialType getSpecialType(int row, int col);
    int countConsecutive(int row, int col, int dx, int dy, int itemType);
    bool isLPattern(int row, int col, int left, int right, int up, int down);
    bool isTPattern(int row, int col, int left, int right, int up, int down);
    vector<MatchResult> findAllMatchesWithPatterns();
    int processCascadeWithSpecials();
    bool swap(int row1, int col1, int row2, int col2);
};
#endif //MATCH3ENGINE_MATCH3_ENGINE_H
