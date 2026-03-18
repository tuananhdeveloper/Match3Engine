#include <unordered_map>

enum class JniType {
    MOVE,
    MATCH_RESULT,
    INIT_SET,
    ADD_SET,
    PAIR,
    INTEGER
};

struct JniAsset {
    jclass clazz;
    jmethodID method;
};

std::unordered_map<JniType, JniAsset> g_jni_map;

void register_jni_type(JNIEnv* env, JniType type, const char* path, const char* methodName, const char* signature) {
    jclass localRef = env->FindClass(path);
    if (!localRef) {
        return;
    }
    JniAsset asset;
    asset.clazz = (jclass)env->NewGlobalRef(localRef);
    asset.method = env->GetMethodID(asset.clazz, methodName, signature);
    g_jni_map[type] = asset;
    env->DeleteLocalRef(localRef);
}

void clear_jni_cache(JNIEnv* env) {
    for (auto const& [type, asset] : g_jni_map) {
        if (asset.clazz) {
            env->DeleteGlobalRef(asset.clazz);
        }
    }
    g_jni_map.clear();
}

jobject create_jni_object(JNIEnv* env, JniType type, ...) {
    auto it = g_jni_map.find(type);
    if (it == g_jni_map.end()) {
        return nullptr;
    }
    va_list args;
    va_start(args, type);
    jobject obj = env->NewObjectV(it->second.clazz, it->second.method, args);
    va_end(args);
    return obj;
}

jobject create_pair(JNIEnv* env, jobject firstObj, jobject secondObj) {
    return create_jni_object(env, JniType::PAIR, firstObj, secondObj);
}

jobject create_integer(JNIEnv* env, int value) {
    return create_jni_object(env, JniType::INTEGER, value);
}

jobject create_java_pair(JNIEnv* env, int first, int second) {
    jobject firstObj = create_integer(env, first);
    jobject secondObj = create_integer(env, second);
    jobject pairObj = create_pair(env, firstObj, secondObj);
    env->DeleteLocalRef(firstObj);
    env->DeleteLocalRef(secondObj);
    return pairObj;
}

jobject create_java_set_of_pairs(JNIEnv* env, std::set<pair<int, int>> items) {
    auto it = g_jni_map.find(JniType::INIT_SET);
    auto it2 = g_jni_map.find(JniType::ADD_SET);
    if (it == g_jni_map.end() || it2 == g_jni_map.end()) {
        return nullptr;
    }
    jobject setObj = env->NewObject(it->second.clazz, it->second.method);
    for (auto item: items) {
        jobject pairObj = create_java_pair(env, item.first, item.second);
        env->CallBooleanMethod(setObj, it2->second.method, pairObj);
    }
    return setObj;
}

