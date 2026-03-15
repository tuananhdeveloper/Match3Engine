#include <jni.h>
#include <set>
#include <vector>
#include "match3_engine.h"

Match3Engine* engine = nullptr;

void init(JNIEnv *env, jobject thiz,
          int width, int height, int itemTypes) {
    if (engine == nullptr) {
        engine = new Match3Engine(width, height, itemTypes);
    }
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
            grid[row][col].specialType = static_cast<SpecialType>(data[index++]);
        }
    }

    env->ReleaseIntArrayElements(flatData, data, JNI_ABORT);
    engine->setGrid(grid);
}

static JNINativeMethod method_table[] = {
        {"init", "(III)V", (void*)init},

        {"setGrid", "([III)V", (void*)setGrid},

        {"findAllMatches", "()[I", (jintArray*)findAllMatches}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) return JNI_ERR;

    const char* classNames[] = {
            "com/tuananh/match3/bridge/DesktopNativeLib",
            "com/tuananh/match3/bridge/AndroidNativeLib"
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