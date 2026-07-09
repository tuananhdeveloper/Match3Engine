package com.tuananh.match3_engine_sdk;

import java.util.Set;

public class MatchResult {
    private int pattern;
    private Set<Pair<Integer, Integer>> cells;
    private Pair<Integer, Integer> epicenter;
    private int itemType;

    public MatchResult(int pattern, Set<Pair<Integer, Integer>> cells, Pair<Integer, Integer> epicenter, int itemType) {
        this.pattern = pattern;
        this.cells = cells;
        this.epicenter = epicenter;
        this.itemType = itemType;
    }

    public MatchPattern getPattern() {
        return MatchPattern.values()[pattern];
    }

    public Set<Pair<Integer, Integer>> getCells() {
        return cells;
    }

    public Pair<Integer, Integer> getEpicenter() {
        return epicenter;
    }

    public int getItemType() {
        return itemType;
    }

    @Override
    public String toString() {
        return "MatchResult{" +
            "pattern=" + getPattern() +
            ", cells=" + cells +
            ", epicenter=" + epicenter +
            ", itemType=" + itemType +
            '}';
    }
}
