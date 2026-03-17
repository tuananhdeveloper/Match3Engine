#include <unordered_map>

enum class JniType {

};

struct JniAsset {
    jclass clazz;
    jmethodID constructor;
};

std::unordered_map<JniType, JniAsset> g_jni_map;

void register_jni_type(JNIEnv* env, JniType type, const char* path, const char* methodName, const char* signature) {
    jclass localRef = env->FindClass(path);
    if (!localRef) {
        return;
    }
    JniAsset asset;
    asset.clazz = (jclass)env->NewGlobalRef(localRef);
    asset.constructor = env->GetMethodID(asset.clazz, methodName, signature);
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
    jobject obj = env->NewObjectV(it->second.clazz, it->second.constructor, args);
    va_end(args);
    return obj;
}