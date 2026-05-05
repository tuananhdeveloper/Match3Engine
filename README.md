Match3 Engine

- [x] Match Detection (3-MATCH, 4-MATCH, 5-MATCH, L-MATCH, T-MATCH)
- [x] Gravity
- [x] Refill
- [x] Cascade
- [x] Swap
- [x] Deadlock Detection
- [x] Hint System
- [x] Special Candies
- [x] Shuffle

# Match-3 - Scoring System Logic

This document outlines the scoring mechanics implemented within the **Match-3 Engine (C++)**. The system is designed to reward tactical play and chain reactions.

---

## 1. General Scoring Formula

Each time a match or an explosion occurs, the score is calculated as follows:

`Points = (MatchCount * BasePerItem) * ComboMultiplier`

* **MatchCount**: The number of items destroyed in a single explosion or match.
* **BasePerItem**: The point value assigned to each item type.
* **ComboMultiplier**: A multiplier that increments after every chain reaction (gravity drop leading to new matches).

---

## 2. Base Metrics

| Item Type / Action | BasePerItem
| :--- | :---: | :--- |
| **Normal Item** | 10
| **Special Item** | 20
| **Transformation** | 20

---

## 3. Special Combo Logic (Machine Combos)

When an **COLOR_BOMB** is swapped with other items, the score is calculated in two distinct phases:

### Phase A: Transformation
This rewards the player for converting normal items into special items.
* **Logic**: Only counts **normal items** that are changed. Existing special items on the board are not counted for this bonus.
* **Formula**: `(ConvertedCount * 20) * ComboMultiplier`

### Phase B: Activation (Mass Explosion)
Points are awarded as each newly created (and existing) special item detonates.
* **Striped Vertical/Horizontal Activation (Board: 4 x 4)**: Calculated as 7 (4 vertical items, 3 horizontal) tiles destroyed (`7 * 20`).
* **WRAPPED Activation**: Calculated as 9 tiles destroyed (`9 * 20`).

---

## 4. Combo Multiplier Mechanics

* **Start of Move**: `ComboMultiplier` is set to **0**.
* **Chain Reaction**: After the board settles and a new automatic match/explosion occurs, the `ComboMultiplier` increases by **1**.
If transformation, ComboMultiplier is always 1.