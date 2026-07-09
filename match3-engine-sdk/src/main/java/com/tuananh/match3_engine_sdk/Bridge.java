package com.tuananh.match3_engine_sdk;

import java.util.Map;

public interface Bridge {
    void init(int width, int height, int[] itemTypes);
    void setGrid(Cell[][] grid);
    int[] findAllMatches();
    int getItem(int row, int col);
    void applyGravity();
    int processCascade();
    boolean hasValidMoves();
    void shuffle();
    int countValidMoves();
    Move findHint();
    MatchResult detectPatternAt(int row, int col);
    int analyzeMatchPattern(int row, int col, int left, int right, int up, int down);
    void spawnSpecialCell(MatchResult matchResult);
    int getSpecialType(int row, int col);
    int countConsecutive(int row, int col, int dx, int dy, int itemType);
    boolean isLPattern(int row, int col, int left, int right, int up, int down);
    boolean isTPattern(int row, int col, int left, int right, int up, int down);
    MatchResult[] findAllMatchesWithPatterns();
    int processCascadeWithSpecials();
    boolean swap(int row1, int col1, int row2, int col2);
    int swapCollectEvents(int row1, int col1, int row2, int col2, int[] outEvents);
    int stepCollectEvents(boolean isStreaming, int[] outEvents);
    void reset();
    void setHoleItemId(int id);
    void setSpecialTypeMap(Map<Integer, Integer> map);
    void setSpecialIndexMap(Map<Pair<Integer, Integer>, Integer> map);
    void updateBasePoint(int normalBasePoint, int specialBasePoint);
    int getTotalScore();
    PlayerProgress loadGame(String path);
    void onUpdatePlayerProgress(boolean isWin, int levelId, int score);
    Cell[] getRemovedCells();
    void updateSettings(boolean isMusicEnabled, boolean isSfxEnabled, float volume);
}
