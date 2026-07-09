package com.tuananh.match3_engine_sdk;

import java.util.Map;

public class NativeEngine implements Bridge {
    private native void nativeInit(int width, int height, int[] itemTypes);
    private native void nativeSetGrid(int[] flatData, int rows, int cols);
    private native int[] nativeFindAllMatches();
    private native int nativeGetItem(int row, int col);
    private native void nativeApplyGravity();
    private native int nativeProcessCascade();
    private native boolean nativeHasValidMoves();
    private native void nativeShuffle();
    private native int nativeCountValidMoves();
    private native Move nativeFindHint();
    private native MatchResult nativeDetectPatternAt(int row, int col);
    private native int nativeAnalyzeMatchPattern(int row, int col, int left, int right, int up, int down);
    private native void nativeSpawnSpecialCell(MatchResult matchResult);
    private native int nativeGetSpecialType(int row, int col);
    private native int nativeCountConsecutive(int row, int col, int dx, int dy, int itemType);
    private native boolean nativeIsLPattern(int row, int col, int left, int right, int up, int down);
    private native boolean nativeIsTPattern(int row, int col, int left, int right, int up, int down);
    private native MatchResult[] nativeFindAllMatchesWithPatterns();
    private native int nativeProcessCascadeWithSpecials();
    private native boolean nativeSwap(int row1, int col1, int row2, int col2);
    private native int nativeSwapCollectEvents(int row1, int col1, int row2, int col2, int[] outEvents);
    private native int nativeStepCollectEvents(boolean isStreaming, int[] outEvents);
    private native void nativeReset();
    private native void nativeSetSpecialTypeMap(int[] keys, int[] values, int size);
    private native void nativeSetHoleItemId(int id);
    private native void nativeSetSpecialIndexMap(Pair<Integer, Integer>[] keys, int[] values, int size);
    private native void nativeUpdateBasePoint(int normalBasePoint, int specialBasePoint);
    private native int nativeGetTotalScore();
    private native PlayerProgress nativeLoadGame(String path);
    private native void nativeOnUpdatePlayerProgress(boolean isWin, int levelId, int score);
    private native Cell[] nativeGetRemovedCells();
    private native void nativeUpdateSettings(boolean isMusicEnabled, boolean isSfxEnabled, float volume);

    @Override
    public void init(int width, int height, int[] itemTypes) {
        nativeInit(width, height, itemTypes);
    }

    @Override
    public void setGrid(Cell[][] grid) {
        int rows = grid.length;
        int cols = grid[0].length;
        int flatDataSize = rows * cols * 2;
        int[] flatData = new int[flatDataSize];
        int index = 0;

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                flatData[index++] = grid[row][col].getType();
                flatData[index++] = grid[row][col].getSpecialType().getId();
            }
        }

        nativeSetGrid(flatData, rows, cols);
    }

    @Override
    public int[] findAllMatches() {
        return nativeFindAllMatches();
    }

    @Override
    public int getItem(int row, int col) {
        return nativeGetItem(row, col);
    }

    @Override
    public void applyGravity() {
        nativeApplyGravity();
    }

    @Override
    public int processCascade() {
        return nativeProcessCascade();
    }

    @Override
    public boolean hasValidMoves() {
        return nativeHasValidMoves();
    }

    @Override
    public void shuffle() {
        nativeShuffle();
    }

    @Override
    public int countValidMoves() {
        return nativeCountValidMoves();
    }

    @Override
    public Move findHint() {
        return nativeFindHint();
    }

    @Override
    public MatchResult detectPatternAt(int row, int col) {
        return nativeDetectPatternAt(row, col);
    }

    @Override
    public int analyzeMatchPattern(int row, int col, int left, int right, int up, int down) {
        return nativeAnalyzeMatchPattern(row, col, left, right, up, down);
    }

    @Override
    public void spawnSpecialCell(MatchResult matchResult) {
        nativeSpawnSpecialCell(matchResult);
    }

    @Override
    public int getSpecialType(int row, int col) {
        return nativeGetSpecialType(row, col);
    }

    @Override
    public int countConsecutive(int row, int col, int dx, int dy, int itemType) {
        return nativeCountConsecutive(row, col, dx, dy, itemType);
    }

    @Override
    public boolean isLPattern(int row, int col, int left, int right, int up, int down) {
        return nativeIsLPattern(row, col, left, right, up, down);
    }

    @Override
    public boolean isTPattern(int row, int col, int left, int right, int up, int down) {
        return nativeIsTPattern(row, col, left, right, up, down);
    }

    @Override
    public MatchResult[] findAllMatchesWithPatterns() {
        return nativeFindAllMatchesWithPatterns();
    }

    @Override
    public int processCascadeWithSpecials() {
        return nativeProcessCascadeWithSpecials();
    }

    @Override
    public boolean swap(int row1, int col1, int row2, int col2) {
        return nativeSwap(row1, col1, row2, col2);
    }

    @Override
    public int swapCollectEvents(int row1, int col1, int row2, int col2, int[] outEvents) {
        return nativeSwapCollectEvents(row1, col1, row2, col2, outEvents);
    }

    @Override
    public int stepCollectEvents(boolean isStreaming, int[] outEvents) {
        return nativeStepCollectEvents(isStreaming, outEvents);
    }

    @Override
    public void reset() {
        nativeReset();
    }

    @Override
    public void setHoleItemId(int id) {
        nativeSetHoleItemId(id);
    }

    @Override
    public void setSpecialTypeMap(Map<Integer, Integer> map) {
        int size = map.size();
        int[] keys = new int[size];
        int[] values = new int[size];
        int i = 0;
        for (Map.Entry<Integer, Integer> entry : map.entrySet()) {
            keys[i] = entry.getKey();
            values[i] = entry.getValue();
            i++;
        }
        nativeSetSpecialTypeMap(keys, values, size);
    }

    @Override
    public void setSpecialIndexMap(Map<Pair<Integer, Integer>, Integer> map) {
        int size = map.size();
        Pair<Integer, Integer>[] keys = new Pair[size];
        int[] values = new int[size];
        int i = 0;
        for (Map.Entry<Pair<Integer,Integer>, Integer> entry : map.entrySet()) {
            keys[i] = entry.getKey();
            values[i] = entry.getValue();
            i++;
        }
        nativeSetSpecialIndexMap(keys, values, size);
    }

    @Override
    public void updateBasePoint(int normalBasePoint, int specialBasePoint) {
        nativeUpdateBasePoint(normalBasePoint, specialBasePoint);
    }

    @Override
    public int getTotalScore() {
        return nativeGetTotalScore();
    }

    @Override
    public PlayerProgress loadGame(String path) {
        return nativeLoadGame(path);
    }

    @Override
    public void onUpdatePlayerProgress(boolean isWin, int levelId, int score) {
        nativeOnUpdatePlayerProgress(isWin, levelId, score);
    }

    @Override
    public Cell[] getRemovedCells() {
        return nativeGetRemovedCells();
    }

    @Override
    public void updateSettings(boolean isMusicEnabled, boolean isSfxEnabled, float volume) {
        nativeUpdateSettings(isMusicEnabled, isSfxEnabled, volume);
    }
}
