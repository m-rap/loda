#include <jni.h>
#include <string>
#include <dlfcn.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_mrap_loda_Loda_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

void jniprintf(JNIEnv* env, jbyteArray jBuff, jlongArray jBuffTime, const char* fmt, ...) {
    char* buff = (char*)env->GetByteArrayElements(jBuff, (jboolean*)0);

    jlong* buffTime = env->GetLongArrayElements(jBuffTime, (jboolean*)0);

    va_list args;
    va_start(args, fmt);
    sprintf(buff, fmt, args);
    va_end(args);

    timeval t;
    gettimeofday(&t, NULL);
    buffTime[0] = t.tv_sec * 1000 + t.tv_usec / 1000;

    env->ReleaseByteArrayElements(jBuff, (jbyte*)buff, 0);

    env->ReleaseLongArrayElements(jBuffTime, buffTime, 0);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_mrap_loda_Loda_loadLib(
        JNIEnv* env,
        jobject /* this */, jstring jLibPath, jbyteArray jBuff, jlongArray jBuffTime) {
    jniprintf(env, jBuff, jBuffTime, "inside loadLib\n");
    const char* libPath = env->GetStringUTFChars(jLibPath, (jboolean*)0);
    void* handle = dlopen(libPath, RTLD_LAZY);
    if (!handle) {
        jniprintf(env, jBuff, jBuffTime, "dlopen error\n");
        return env->NewStringUTF(dlerror());
    }
    jniprintf(env, jBuff, jBuffTime, "dlopen success\n");
    void (*execute)();
    execute = (void (*)())dlsym(handle, "execute");
    char* error = dlerror();
    if (error != NULL) {
        return env->NewStringUTF(error);
    }
    jniprintf(env, jBuff, jBuffTime, "dlsym success\n");
    execute();
    dlclose(handle);
    env->ReleaseStringUTFChars(jLibPath, libPath);
    jniprintf(env, jBuff, jBuffTime, "handle closed\n");
    return env->NewStringUTF("success");
}