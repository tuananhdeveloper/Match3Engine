package com.tuananh.match3_engine_sdk;

public class Cell {
    private int type;
    private SpecialType specialType;

    public Cell() {
        this.type = -1;
        this.specialType = SpecialType.NONE;
    }
    public Cell(int type) {
        this.type = type;
        this.specialType = SpecialType.NONE;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getType() {
        return type;
    }

    public void setSpecialType(SpecialType specialType) {
        this.specialType = specialType;
    }

    public SpecialType getSpecialType() {
        return specialType;
    }

    public static Cell[][] getCells(int[][] data) {
        Cell[][] cells = new Cell[data.length][data[0].length];
        for (int row = 0; row < data.length; row++) {
            for (int col = 0; col < data[0].length; col++) {
                int itemId = data[row][col];
                cells[row][col] = new Cell(itemId);
                SpecialType type = SpecialType.getType(itemId);
                if (type != null) {
                    cells[row][col].setSpecialType(SpecialType.getType(itemId));
                }
                else {
                    cells[row][col].setSpecialType(SpecialType.NONE);
                }
            }
        }
        return cells;
    }
}
