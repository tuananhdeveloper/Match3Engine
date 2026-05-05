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

enum SpecialType {
    NONE = -1,
    STRIPED_VERTICAL = 0,
    STRIPED_HORIZONTAL = 1,
    COLOR_BOMB = 2,
    WRAPPED = 3
};

enum class MatchPattern {
    NONE,
    MATCH_3,
    MATCH_4_HORIZONTAL,
    MATCH_4_VERTICAL,
    MATCH_5,
    MATCH_L,
    MATCH_T,
    STRIPED_STRIPED,
    STRIPED_WRAPPED,
    WRAPPED_WRAPPED,
    COLOR_BOMB_NORMAL_TYPE,
    COLOR_BOMB_STRIPED,
    COLOR_BOMB_WRAPPED,
    COLOR_BOMB_COLOR_BOMB
};

struct Cell {
    int type;
    int specialType;
    Cell() : type(-1), specialType(-1) {}
    Cell(int t) : type(t), specialType(-1) {}
};

struct MatchResult {
    MatchPattern pattern;
    set<pair<int, int>> cells;
    pair<int, int> epicenter;
    int itemType;
};

enum class EventType {
    SWAP,
    FALL,
    SPAWN
};

struct EventWriter {
    int* out;
    int capacity;
    int length;
    static const int STRIDE = 8;

    EventWriter(int* out_, int cap_) : out(out_), capacity(cap_), length(0) {}

    bool push8(int type, int row1, int col1, int row2, int col2, int item, int special, int flags) {
        if (length + STRIDE > capacity) {
            return false;
        }
        out[length++] = type;
        out[length++] = row1; out[length++] = col1;
        out[length++] = row2; out[length++] = col2;
        out[length++] = item;
        out[length++] = special;
        out[length++] = flags;
        return true;
    }
};

struct pair_hash {
    inline size_t operator()(const std::pair<int, int> & v) const {
        return v.first * 2026 + v.second;
    }
};

class Match3Engine {
private:
    int width;
    int height;
    vector<int> itemTypes;
    vector<vector<Cell>> grid;
    vector<vector<Cell>> originalGrid;
    const int EMPTY_CELL = -1;
    const int MAX_ATTEMPTS = 100;
    int holeItemId = -1;
    int selectedRow = -1;
    int selectedCol = -1;
    optional<MatchResult> comboResult;
    unordered_map<int, int> specialTypeMap;
    unordered_map<pair<int, int>, int, pair_hash> specialIndexMap;
    vector<int> normalPool;
    vector<int> specialPool;
    int normalItemBasePoint = 10;
    int specialItemBasePoint = 20;
    int totalScore = 0;

private:
    set<pair<int, int>> findHorizontalMatches(int row);
    set<pair<int, int>> findVerticalMatches(int col);
    void refillSmart(EventWriter* writer = nullptr);
    void refillFromTop(EventWriter* writer = nullptr);
    void removeMatches(const set<pair<int, int>>& matches);
    bool wouldCreateMatch(int row, int col, int itemType);
    bool hasVerticalMatchAt(int row, int col);
    bool hasHorizontalMatchAt(int row, int col);
    bool isInBounds(int row, int col);
    bool isAdjacent(int row1, int col1, int row2, int col2);
    bool wouldCreateMatchAfterSwap(int row1, int col1, int row2, int col2);
    bool checkMatchAt(int row, int col);
    void applyGravityAndRefillStream(EventWriter* writer);
    optional<MatchResult> getCombo(int row2, int col2);
    MatchResult getComboMatchResult(int row1, int col1, int row2, int col2);
    MatchPattern findComboPattern(int row1, int col1, int row2, int col2);
    bool isSpecialType(int itemId) const;
    SpecialType getSpecialTypeFromIndex(int index);
    int spawnNewItem();
    void updateScore(int matchCount, bool isSpecial, int comboMultiplier);

public:
    Match3Engine(int width, int height, vector<int> itemTypes);
    set<pair<int, int>> findAllMatches();
    void setGrid(vector<vector<Cell>> grid);
    void setHoleItemId(int holeItemId);
    int getItem(int col, int row);
    void applyGravity(EventWriter* writer = nullptr);
    int processCascade();
    bool hasValidMoves();
    void shuffle();
    int countValidMoves();
    optional<Move> findHint();
    MatchResult detectPatternAt(int row, int col);
    MatchPattern analyzeMatchPattern(int row, int col, int left, int right, int up, int down);
    void spawnSpecialCell(const MatchResult& match);
    int getSpecialType(int row, int col);
    int countConsecutive(int row, int col, int dx, int dy, int itemType);
    bool isLPattern(int row, int col, int left, int right, int up, int down);
    bool isTPattern(int row, int col, int left, int right, int up, int down);
    vector<MatchResult> findAllMatchesWithPatterns();
    int processCascadeWithSpecials(bool streaming = false, bool isRefillingSmart = false, EventWriter* writer = nullptr);
    bool swap(int row1, int col1, int row2, int col2);
    void reset();
    void setSpecialTypeMap(unordered_map<int, int> specialTypeMap);
    void setSpecialIndexMap(unordered_map<pair<int, int>, int, pair_hash> specialIndexMap);
    void updateBasePoint(int normalItemBasePoint, int specialItemBasePoint);
    int getTotalScore();
};
#endif //MATCH3ENGINE_MATCH3_ENGINE_H
