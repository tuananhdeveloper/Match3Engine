package com.tuananh.match3_engine_sdk;

public class Move {
    private int row1;
    private int col1;
    private int row2;
    private int col2;

    public Move(int row1, int col1, int row2, int col2) {
        this.row1 = row1;
        this.col1 = col1;
        this.row2 = row2;
        this.col2 = col2;
    }

    public int getRow1() {
        return row1;
    }

    public int getCol1() {
        return col1;
    }

    public int getRow2() {
        return row2;
    }

    public int getCol2() {
        return col2;
    }

    @Override
    public String toString() {
        return "Move{" +
            "row1=" + row1 +
            ", col1=" + col1 +
            ", row2=" + row2 +
            ", col2=" + col2 +
            '}';
    }
}
