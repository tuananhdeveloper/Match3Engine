package com.tuananh.match3_engine_sdk;

import java.util.HashMap;
import java.util.Map;

public enum SpecialType {
    NONE(-1),
    STRIPED_VERTICAL(0), // Vertical Drip Filter
    STRIPED_HORIZONTAL(1), // Horizontal Drip Filter
    COLOR_BOMB(2), // Ultimate Espresso Machine
    WRAPPED(3); // Coffee Shaker
    static final Map<Integer, Integer> specialTypeMap = new HashMap<>(); // Key: item index, Value: Special Type id
    static final Map<Pair<Integer, Integer>, Integer> specialIndexMap = new HashMap<>(); // Key: {Item index, special type id}, Value: item index

    static {
        for (int value = 0; value <= 5; value++) {
            specialTypeMap.put(value, SpecialType.NONE.getId());
        }
        for (int value = 6; value <= 11; value++) {
            specialTypeMap.put(value, SpecialType.STRIPED_VERTICAL.getId());
        }
        for (int value = 12; value <= 17; value++) {
            specialTypeMap.put(value, SpecialType.STRIPED_HORIZONTAL.getId());
        }
        specialTypeMap.put(18, SpecialType.COLOR_BOMB.getId());
        for (int value = 19; value <= 24; value++) {
            specialTypeMap.put(value, SpecialType.WRAPPED.getId());
        }

        int startAtlasIndex = 6;
        for (int index = 0; index <= 5; index++) {
            specialIndexMap.put(new Pair<>(index, SpecialType.STRIPED_VERTICAL.getId()), startAtlasIndex);
            specialIndexMap.put(new Pair<>(index, SpecialType.STRIPED_HORIZONTAL.getId()), startAtlasIndex + 6);
            specialIndexMap.put(new Pair<>(index, SpecialType.WRAPPED.getId()), startAtlasIndex + 13);
            specialIndexMap.put(new Pair<>(index, SpecialType.COLOR_BOMB.getId()), 18);
            startAtlasIndex++;
        }

    }

    private final int id;

    SpecialType(int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    public static SpecialType getType(int id) {
        for(SpecialType type: SpecialType.values()) {
            if (type.getId() == specialTypeMap.get(id)) {
                return type;
            }
        }
        return null;
    }

    public static Map<Integer, Integer> getSpecialTypeMap() {
        return specialTypeMap;
    }

    public static Map<Pair<Integer, Integer>, Integer> getSpecialIndexMap() {
        return specialIndexMap;
    }
}
