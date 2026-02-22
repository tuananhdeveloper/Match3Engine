//
// Created by Nguyễn Tuấn Anh on 15/2/26.
//
#include <android_native_app_glue.h>
#include "match3_engine.h"
#include <iostream>
#include <cassert>
#include <android/log.h>
#define LOG_TAG "MyAppTag" // Replace "MyAppTag" with your desired tag
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void testHorizontalMatch() {
    Match3Engine engine(5, 5, 3);

    // Setup known grid
    engine.setGrid({
                           {0, 0, 0, 1, 2},
                           {1, 2, 1, 2, 1},
                           {2, 1, 2, 1, 2},
                           {1, 2, 1, 2, 1},
                           {2, 1, 2, 1, 2}
                   });

    auto matches = engine.findAllMatches();

    assert(matches.count({0, 0}) == 1);
    assert(matches.count({0, 1}) == 1);
    assert(matches.count({0, 2}) == 1);

    LOGD("✓ Horizontal match test passed\n");
}

void testGravity() {
    Match3Engine engine(3, 3, 2);

    engine.setGrid({
                           {0, -1,  1},  // A  .  B
                           {-1, 1, -1},  // .  B  .
                           {1,  0,  0}   // B  A  A
                   });
    engine.applyGravity();

    // Expected result:
    // .  .  .
    // A  B  B
    // B  A  A

    // Column 0: [EMPTY, A, B]
    assert(engine.getItem(0, 0) == -1);  // EMPTY
    assert(engine.getItem(0, 1) == 0);   // A
    assert(engine.getItem(0, 2) == 1);   // B ✓ STILL HERE!

    // Column 1: [EMPTY, B, A]
    assert(engine.getItem(1, 0) == -1);
    assert(engine.getItem(1, 1) == 1);
    assert(engine.getItem(1, 2) == 0);

    // Column 2: [EMPTY, B, A]
    assert(engine.getItem(2, 0) == -1);
    assert(engine.getItem(2, 1) == 1);
    assert(engine.getItem(2, 2) == 0);

    LOGD("\n✓ Gravity test PASSED\n");
}

void testCascade() {
    Match3Engine engine(6, 6, 4);

    engine.setGrid({
                           {0, 0, 0, 1, 2, 3},
                           {1, 2, 1, 2, 3, 1},
                           {1, 3, 2, 3, 1, 2},
                           {2, 1, 3, 1, 2, 3},
                           {1, 2, 1, 2, 3, 1},
                           {3, 1, 2, 3, 1, 2}
                   });
    assert(engine.processCascade() == 1);
    LOGD("✓ Cascade test passed\n");
}

void testHasValidMoves() {
    Match3Engine engine(5, 3, 3);
    engine.setGrid({
                           {0, 1, 0, 1, 2},  // A B A B C
                           {1, 0, 1, 0, 1},  // B A B A B
                           {2, 1, 2, 1, 0}   // C B C B A
                   });

    assert(engine.hasValidMoves() == true);
    LOGD("✓ Has valid moves\n");
}

void testNoValidMoves() { // no valid moves, small grid
    Match3Engine engine(3, 3, 3);
    engine.setGrid({
        {2, 1, 0},
        {0, 1, 2},
        {0, 2, 0}
    });
    assert(engine.hasValidMoves() == false);
    LOGD("✓ Test No valid moves passed\n");
}

void testMultipleValidMoves() {
    Match3Engine engine(5, 5, 3);
    engine.setGrid({
       {0, 1, 0, 1, 2},
       {1, 0, 0, 2, 1},
       {2, 1, 2, 1, 0},
       {0, 2, 1, 0, 0},
       {1, 0, 2, 1, 2}
    });

    assert(engine.hasValidMoves() == true);
    int validMoveCount = engine.countValidMoves();
    LOGD("Number of valid moves: %d", validMoveCount);
    LOGD("✓ Test Multiple Valid Moves passed");
}

void testFindHint() {
    Match3Engine engine(5, 5, 3);
    engine.setGrid({
        {0, 1, 0, 1, 2},
        {1, 0, 1, 0, 1},
        {2, 1, 2, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 2}
    });

    auto hint = engine.findHint();
    assert(hint.has_value() == true);
    LOGD("✓ Test Hint: (%d, %d), (%d, %d)", hint->row1, hint->col1, hint->row2, hint->col2);
}

void testFourMatchHorizontal() {
    Match3Engine engine(6, 5, 3);
    engine.setGrid({
        {1, 2, 1, 2, 1, 2},
        {0, 0, 0, 0, 2, 1},
        {2, 1, 2, 1, 0, 2},
        {1, 2, 1, 2, 1, 0},
        {0, 1, 0, 1, 2, 1}
    });
    auto match = engine.detectPatternAt(1, 1);
    if (match.pattern == MatchPattern::MATCH_4_HORIZONTAL) {
        engine.spawnSpecialCell(match);
        auto specialType = engine.getSpecialType(match.epicenter.first,
                                                 match.epicenter.second);
        assert(specialType == SpecialType::STRIPED_HORIZONTAL);
        LOGD("✓ Striped Horizontal candy spawned!\n");
    }
    else {
        LOGD("Failed\n");
    }
}

void testLMatch() {
    Match3Engine engine(5, 5, 3);
    engine.setGrid({
        {0, 0, 0, 1, 2},
        {2, 1, 0, 2, 1},
        {1, 2, 0, 1, 2},
        {0, 1, 2, 1, 0},
        {2, 0, 1, 2, 1}
    });
    auto match = engine.detectPatternAt(0, 2);
    if (match.pattern == MatchPattern::MATCH_L) {
        LOGD("✓ L-MATCH detected!\n");
        LOGD("Cells: %d\n", match.cells.size());
        engine.spawnSpecialCell(match);
        auto specialType = engine.getSpecialType(0, 2);
        assert(specialType == SpecialType::WRAPPED);
        LOGD("✓ Wrapped candy spawned!\n");
    }
}

void testTMatch() {
    Match3Engine engine(5, 5, 3);
    engine.setGrid({
        {1, 2, 0, 1, 2},
        {2, 1, 0, 2, 1},
        {0, 0, 0, 0, 2},
        {1, 2, 1, 2, 1},
        {2, 1, 2, 1, 0}
    });
    auto match = engine.detectPatternAt(2, 2);
    if (match.pattern == MatchPattern::MATCH_T) {
        LOGD("✓ T-MATCH detected!\n");
        engine.spawnSpecialCell(match);
        auto specialType = engine.getSpecialType(2, 2);
        assert(specialType == SpecialType::WRAPPED);
        LOGD("✓ Wrapped candy spawned!\n");
    }
}

void test5Match() {
    Match3Engine engine(7, 5, 3);
    engine.setGrid({
        {1, 2, 1, 2, 1, 2, 1},
        {0, 0, 0, 0, 0, 1, 2},
        {2, 1, 2, 1, 2, 0, 1},
        {1, 2, 1, 2, 1, 2, 0},
        {0, 1, 0, 1, 0, 1, 2}
    });
    auto match = engine.detectPatternAt(1, 2);
    if (match.pattern == MatchPattern::MATCH_5) {
        LOGD("✓ 5-MATCH detected!\n");
        LOGD("Cells: %d\n", match.cells.size());
        engine.spawnSpecialCell(match);
        auto specialType = engine.getSpecialType(1, 2);
        assert(specialType == SpecialType::COLOR_BOMB);
        LOGD("✓ Color Bomb spawned!\n");
    }
}

void testCascadeWithSpecials() {
    Match3Engine engine(6, 6, 4);
    engine.setGrid({
        {0, 0, 0, 0, 1, 2},
        {1, 2, 1, 2, 3, 1},
        {2, 3, 2, 3, 1, 2},
        {3, 1, 3, 1, 2, 3},
        {1, 2, 1, 2, 3, 1},
        {2, 3, 2, 3, 1, 2}
    });
    int cascades = engine.processCascadeWithSpecials();

    bool foundSpecial = false;
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
            if (engine.getSpecialType(row, col) != SpecialType::NONE) {
                foundSpecial = true;
                switch (engine.getSpecialType(row, col)) {
                    case SpecialType::STRIPED_HORIZONTAL:
                        LOGD("STRIPED_HORIZONTAL\n");
                        break;
                    case SpecialType::STRIPED_VERTICAL:
                        LOGD("STRIPED_VERTICAL\n");
                        break;
                    case SpecialType::WRAPPED:
                        LOGD("WRAPPED\n");
                        break;
                    case SpecialType::COLOR_BOMB:
                        LOGD("COLOR_BOMB\n");
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (foundSpecial) {
        LOGD("Cascades: %d", cascades);
        LOGD("✓ Special candies created successfully!\n");
    }
}

void testShuffle() {
    Match3Engine engine(3, 3, 3);
    engine.setGrid({
        {2, 1, 0},
        {0, 1, 2},
        {0, 2, 0}
    });
    assert(engine.hasValidMoves() == false);
    engine.shuffle();
    assert(engine.hasValidMoves() == true);
    LOGD("✓ Shuffle created valid moves\n");
}

void android_main(struct android_app* state) {
    testHorizontalMatch();
    testGravity();
    testCascade();
    testHasValidMoves();
    testNoValidMoves();
    testMultipleValidMoves();
    testFindHint();
    testFourMatchHorizontal();
    testLMatch();
    testTMatch();
    test5Match();
    testCascadeWithSpecials();
    testShuffle();
}
