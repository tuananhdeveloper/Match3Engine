#include <jni.h>
#include <set>
#include <vector>
#include "match3_engine.h"
#include "jni_utils.h"
#include "save_manager.h"

Match3Engine* engine = nullptr;

void init(JNIEnv *env, jobject thiz,
          int width, int height, jintArray itemTypes) {
    jsize len = env->GetArrayLength(itemTypes);
    jint *body = env->GetIntArrayElements(itemTypes, 0);
    std::vector<int> myVector(body, body + len);
    env->ReleaseIntArrayElements(itemTypes, body, 0);

    engine = new Match3Engine(width, height, myVector);
}

void setGrid(JNIEnv *env, jobject thiz,
             jintArray flatData, jint rows, jint cols) {
    jint *data = env->GetIntArrayElements(flatData, nullptr);
    if (data == nullptr) {
        return;
    }
    vector<vector<Cell>> grid(rows, vector<Cell>(cols));

    int index = 0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            grid[row][col].type = data[index++];
            grid[row][col].specialType = data[index++];
        }
    }

    env->ReleaseIntArrayElements(flatData, data, JNI_ABORT);
    engine->setGrid(grid);
}

jintArray findAllMatches(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return nullptr;
    }
    set<pair<int, int>> allMatches = engine->findAllMatches();
    int arraySize = allMatches.size() * 2;
    jintArray result = env->NewIntArray(arraySize);
    if (result == nullptr) {
        return nullptr;
    }
    vector<jint> buffer;
    buffer.reserve(arraySize);
    for (const auto &p: allMatches) {
        buffer.push_back(p.first);
        buffer.push_back(p.second);
    }
    env->SetIntArrayRegion(result, 0, arraySize, buffer.data());
    return result;
}

int getItem(JNIEnv *env, jobject thiz, int row, int col) {
    if (!engine) {
        return -1;
    }
    return engine->getItem(col, row);
}

void applyGravity(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return;
    }
    engine->applyGravity();
}

int processCascade(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return -1;
    }
    return engine->processCascade();
}

bool hasValidMoves(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return false;
    }
    return engine->hasValidMoves();
}

void shuffleGrid(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return;
    }
    engine->shuffle();
}

int countValidMoves(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return -1;
    }
    return engine->countValidMoves();
}

jobject findHint(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return nullptr;
    }
    optional<Move> hint = engine->findHint();
    if (!hint.has_value()) {
        return nullptr;
    }
    jobject localObj = create_jni_object(env, JniType::MOVE, hint->row1, hint->col1, hint->row2, hint->col2);
    return localObj;
}

jobject detectPatternAt(JNIEnv *env, jobject thiz, int row, int col) {
    if (!engine) {
        return nullptr;
    }
    MatchResult result = engine->detectPatternAt(row, col);
    return create_jni_object(env, JniType::MATCH_RESULT,
                             static_cast<int>(result.pattern),
                             create_java_set_of_pairs(env, result.cells),
                             create_java_pair(env, result.epicenter.first, result.epicenter.second),
                             result.itemType);
}

int analyzeMatchPattern(JNIEnv *env, jobject thiz,
                        int row, int col, int left, int right, int up, int down) {
    if (!engine) {
        return -1;
    }
    MatchPattern pattern = engine->analyzeMatchPattern(row, col, left, right, up, down);
    return static_cast<int>(pattern);
}

void spawnSpecialCell(JNIEnv *env, jobject thiz, const MatchResult& match) {
    if (!engine) {
        return;
    }
    engine->spawnSpecialCell(match);
}

int getSpecialType(JNIEnv *env, jobject thiz, int row, int col) {
    if (!engine) {
        return -1;
    }
    return static_cast<int>(engine->getSpecialType(row, col));
}

int countConsecutive(JNIEnv *env, jobject thiz,
                     int row, int col, int dx, int dy, int itemType) {
    if (!engine) {
        return -1;
    }
    return engine->countConsecutive(row, col, dx, dy, itemType);
}

bool isLPattern(JNIEnv *env, jobject thiz,
                int row, int col, int left, int right, int up, int down) {
    if (!engine) {
        return false;
    }
    return engine->isLPattern(row, col, left, right, up, down);
}

bool isTPattern(JNIEnv *env, jobject thiz,
                int row, int col, int left, int right, int up, int down) {
    if (!engine) {
        return false;
    }
    return engine->isLPattern(row, col, left, right, up, down);
}

jobjectArray findAllMatchesWithPatterns(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return nullptr;
    }
    jclass matchResultClass = g_jni_map[JniType::MATCH_RESULT].clazz;
    vector<MatchResult> result = engine->findAllMatchesWithPatterns();
    jobjectArray array = env->NewObjectArray(result.size(), matchResultClass, nullptr);
    for (int i = 0; i < result.size(); i++) {
        jobject item = create_jni_object(env, JniType::MATCH_RESULT,
                                         static_cast<int>(result[i].pattern),
                                         create_java_set_of_pairs(env, result[i].cells),
                                         create_java_pair(env, result[i].epicenter.first, result[i].epicenter.second),
                                         result[i].itemType);
        env->SetObjectArrayElement(array, i, item);
        env->DeleteLocalRef(item);
    }
    return array;
}

int processCascadeWithSpecials(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return -1;
    }
    return engine->processCascadeWithSpecials();
}

bool swapCells(JNIEnv *env, jobject thiz,
          int row1, int col1, int row2, int col2) {
    if (!engine) {
        return false;
    }
    return engine->swap(row1, col1, row2, col2);
}

int swapCollectEvents(JNIEnv *env, jobject thiz,
        int row1, int col1, int row2, int col2, jintArray outEvents) {
    if (!engine) {
        return -1;
    }
    jint *out = env->GetIntArrayElements(outEvents, NULL);
    jsize capacity = env->GetArrayLength(outEvents);

    EventWriter writer = EventWriter(out, (int)capacity);
    bool ok = engine->swap(row1, col1, row2, col2);
    if (ok) {
        writer.push8(static_cast<int>(EventType::SWAP), row1, col1, row2, col2, -1, -1, 0);
    }
    env->ReleaseIntArrayElements(outEvents, out, 0);

    return writer.length;
}

int stepCollectEvents(JNIEnv *env, jobject thiz, jboolean streaming, jintArray outEvents) {
    if (!engine) {
        return -1;
    }
    jint *out = env->GetIntArrayElements(outEvents, NULL);
    jsize capacity = env->GetArrayLength(outEvents);

    EventWriter* writer = new EventWriter(out, (int)capacity);
    bool isStreaming = streaming == JNI_TRUE;
    engine->processCascadeWithSpecials(isStreaming, false, writer);
    env->ReleaseIntArrayElements(outEvents, out, 0);
    return writer->length;
}

void reset(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return;
    }
    engine->reset();
}

void setSpecialTypeMap(JNIEnv *env, jobject thiz, jintArray keys, jintArray values, jint size) {
    if (!engine) {
        return;
    }
    jint* c_keys = env->GetIntArrayElements(keys, NULL);
    jint* c_values = env->GetIntArrayElements(values, NULL);
    unordered_map<int, int> map;
    for (int i = 0; i < size; ++i) {
        map[c_keys[i]] = c_values[i];
    }
    engine->setSpecialTypeMap(map);
    env->ReleaseIntArrayElements(keys, c_keys, 0);
    env->ReleaseIntArrayElements(values, c_values, 0);
}

pair<jint, jint> convertPair(JNIEnv *env, jobject javaPairObj) {
    jclass pairClass = env->GetObjectClass(javaPairObj);
    jmethodID getFirst = env->GetMethodID(pairClass, "getFirst", "()Ljava/lang/Object;");
    jmethodID getSecond = env->GetMethodID(pairClass, "getSecond", "()Ljava/lang/Object;");

    jobject firstObj = env->CallObjectMethod(javaPairObj, getFirst);
    jobject secondObj = env->CallObjectMethod(javaPairObj, getSecond);

    jclass integerClass = env->FindClass("java/lang/Integer");
    jmethodID intValue = env->GetMethodID(integerClass, "intValue", "()I");

    jint first = env->CallIntMethod(firstObj, intValue);
    jint second = env->CallIntMethod(secondObj, intValue);

    env->DeleteLocalRef(firstObj);
    env->DeleteLocalRef(secondObj);

    return std::make_pair(first, second);
}

void setSpecialIndexMap(JNIEnv *env, jobject thiz, jobjectArray keys, jintArray values, jint size) {
    if (!engine) {
        return;
    }
    jint* c_values = env->GetIntArrayElements(values, NULL);
    unordered_map<pair<int, int>, int, pair_hash> map;
    for (int i = 0; i < size; ++i) {
        jobject key = env->GetObjectArrayElement(keys, i);
        map[convertPair(env, key)] = c_values[i];
    }
    engine->setSpecialIndexMap(map);
    env->ReleaseIntArrayElements(values, c_values, 0);
}

void setHoleItemId(JNIEnv *env, jobject thiz, int itemId) {
    if (!engine) {
        return;
    }
    engine->setHoleItemId(itemId);
}

void updateBasePoint(JNIEnv *env, jobject thiz, int normalBasePoint, int specialBasePoint) {
    if (!engine) {
        return;
    }
    engine->updateBasePoint(normalBasePoint, specialBasePoint);
}

int getTotalScore(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return -1;
    }
    return engine->getTotalScore();
}

jobject loadGame(JNIEnv *env, jobject thiz, jstring filePath) {
    PlayerProgress p = _loadGame(from_jstring(env, filePath));

    jsize len = static_cast<jsize>(p.highScores.size());
    jintArray jArray = env->NewIntArray(len);

    if (len > 0 && p.highScores.data() != nullptr) {
        env->SetIntArrayRegion(jArray, 0, len, (const jint*)p.highScores.data());
    }

    jobject obj = create_jni_object(env, JniType::PLAYER_PROGRESS, p.reachedLevel, jArray, p.lastUpdated);
    jfieldID musicFieldID = env->GetFieldID(g_jni_map.find(JniType::PLAYER_PROGRESS)->second.clazz, "isMusicEnabled", "Z");
    jfieldID sfxFieldID = env->GetFieldID(g_jni_map.find(JniType::PLAYER_PROGRESS)->second.clazz, "isSfxEnabled", "Z");
    jfieldID volumeFieldID = env->GetFieldID(g_jni_map.find(JniType::PLAYER_PROGRESS)->second.clazz, "volume", "F");

    env->SetBooleanField(obj, musicFieldID, p.musicEnabled);
    env->SetBooleanField(obj, sfxFieldID, p.sfxEnabled);
    env->SetFloatField(obj, volumeFieldID, p.volume);

    return obj;
}

void onUpdatePlayerProgress(JNIEnv *env, jobject thiz, jboolean isWin, jint levelId, jint score) {
    _onUpdatePlayerProgress(isWin, levelId, score);
}

jobjectArray getRemovedCells(JNIEnv *env, jobject thiz) {
    if (!engine) {
        return nullptr;
    }
    vector<Cell> cells = engine->getRemovedCells();
    jobjectArray array = env->NewObjectArray(cells.size(),
                                             g_jni_map.find(JniType::CELL)->second.clazz,
                                             nullptr);
    for (int i = 0; i < cells.size(); i++) {
        jobject item = create_jni_object(env, JniType::CELL, cells[i].type);
        env->SetObjectArrayElement(array, i, item);
        env->DeleteLocalRef(item);
    }
    return array;
}

void updateSettings(JNIEnv *env, jobject thiz, bool isMusicEnable, bool isSfxEnable, float volume) {
    _updateSettings(isMusicEnable, isSfxEnable, volume);
}

static JNINativeMethod method_table[] = {
        {"nativeInit", "(II[I)V", (void*)init},
        {"nativeSetGrid", "([III)V", (void*)setGrid},
        {"nativeFindAllMatches", "()[I", (void*)findAllMatches},
        {"nativeGetItem", "(II)I", (void*)getItem},
        {"nativeApplyGravity", "()V", (void*)applyGravity},
        {"nativeProcessCascade", "()I", (void*)processCascade},
        {"nativeHasValidMoves", "()Z", (void*)hasValidMoves},
        {"nativeShuffle", "()V", (void*)shuffleGrid},
        {"nativeCountValidMoves", "()I", (void*)countValidMoves},
        {"nativeFindHint", "()Lcom/tinybytelabs/coffeematch3/bridge/Move;", (void*)findHint},
        {"nativeDetectPatternAt", "(II)Lcom/tinybytelabs/coffeematch3/bridge/MatchResult;", (void*)detectPatternAt},
        {"nativeAnalyzeMatchPattern", "(IIIIII)I", (void*)analyzeMatchPattern},
        {"nativeSpawnSpecialCell", "(Lcom/tinybytelabs/coffeematch3/bridge/MatchResult;)V", (void*)spawnSpecialCell},
        {"nativeGetSpecialType", "(II)I", (void*)getSpecialType},
        {"nativeCountConsecutive", "(IIIII)I", (void*) countConsecutive},
        {"nativeIsLPattern", "(IIIIII)Z", (void*)isLPattern},
        {"nativeIsTPattern", "(IIIIII)Z", (void*)isTPattern},
        {"nativeFindAllMatchesWithPatterns", "()[Lcom/tinybytelabs/coffeematch3/bridge/MatchResult;", (void*)findAllMatchesWithPatterns},
        {"nativeProcessCascadeWithSpecials", "()I", (void*)processCascadeWithSpecials},
        {"nativeSwap", "(IIII)Z", (void*)swapCells},
        {"nativeSwapCollectEvents", "(IIII[I)I", (void*)swapCollectEvents},
        {"nativeStepCollectEvents", "(Z[I)I", (void*)stepCollectEvents},
        {"nativeReset", "()V", (void*)reset},
        {"nativeSetHoleItemId", "(I)V", (void*)setHoleItemId},
        {"nativeSetSpecialTypeMap", "([I[II)V", (void*)setSpecialTypeMap},
        {"nativeSetSpecialIndexMap", "([Lcom/tinybytelabs/coffeematch3/bridge/Pair;[II)V", (void*)setSpecialIndexMap},
        {"nativeUpdateBasePoint", "(II)V", (void*)updateBasePoint},
        {"nativeGetTotalScore", "()I", (void*)getTotalScore},
        {"nativeLoadGame", "(Ljava/lang/String;)Lcom/tinybytelabs/coffeematch3/bridge/PlayerProgress;", (void*)loadGame},
        {"nativeOnUpdatePlayerProgress", "(ZII)V", (void*)onUpdatePlayerProgress},
        {"nativeGetRemovedCells", "()[Lcom/tinybytelabs/coffeematch3/bridge/Cell;", (void*)getRemovedCells},
        {"nativeUpdateSettings", "(ZZF)V", (void*) updateSettings}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) return JNI_ERR;

    register_jni_type(env, JniType::MOVE, "com/tinybytelabs/coffeematch3/bridge/Move", "<init>", "(IIII)V");
    register_jni_type(env, JniType::INIT_SET, "java/util/HashSet", "<init>", "()V");
    register_jni_type(env, JniType::ADD_SET, "java/util/HashSet", "add", "(Ljava/lang/Object;)Z");
    register_jni_type(env, JniType::INTEGER, "java/lang/Integer", "<init>", "(I)V");
    register_jni_type(env, JniType::PAIR, "com/tinybytelabs/coffeematch3/bridge/Pair", "<init>", "(Ljava/lang/Object;Ljava/lang/Object;)V");
    register_jni_type(env, JniType::MATCH_RESULT, "com/tinybytelabs/coffeematch3/bridge/MatchResult", "<init>",
                      "(ILjava/util/Set;Lcom/tinybytelabs/coffeematch3/bridge/Pair;I)V");
    register_jni_type(env, JniType::PLAYER_PROGRESS, "com/tinybytelabs/coffeematch3/bridge/PlayerProgress", "<init>", "(I[IJ)V");
    register_jni_type(env, JniType::CELL, "com/tinybytelabs/coffeematch3/bridge/Cell", "<init>", "(I)V");
    const char* classNames[] = {
            "com/tinybytelabs/coffeematch3/lwjgl3/DesktopNativeEngine",
            "com/tinybytelabs/coffeematch3/android/AndroidNativeEngine"
    };

    for (int i = 0; i < 2; i++) {
        jclass clazz = env->FindClass(classNames[i]);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            continue;
        }
        if (clazz) {
            env->RegisterNatives(clazz, method_table, sizeof(method_table) / sizeof(method_table[0]));
        }
    }

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK) {
        clear_jni_cache(env);
    }
}