package com.tuananh.match3_engine_sdk;

public class PlayerProgress {
    int reachedLevel;
    int[] highScores;
    long lastUpdate;
    boolean isMusicEnabled = true;
    boolean isSfxEnabled = true;
    float volume = 1f;

    public PlayerProgress(int reachedLevel, int[] highScores, long lastUpdate) {
        this.reachedLevel = reachedLevel;
        this.highScores = highScores;
        this.lastUpdate = lastUpdate;
    }

    public int getReachedLevel() {
        return reachedLevel;
    }

    public int[] getHighScores() {
        return highScores;
    }

    public long getLastUpdate() {
        return lastUpdate;
    }

    public boolean isMusicEnabled() {
        return isMusicEnabled;
    }

    public boolean isSfxEnabled() {
        return isSfxEnabled;
    }

    public float getVolume() {
        return volume;
    }
}
