#include <jni.h>
#include <string>
#include <dlfcn.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

jmethodID getPrintStreamMetId(JNIEnv* env, jobject printStream) {
    jclass clz = env->GetObjectClass(printStream);
    return env->GetMethodID(clz, "println", "(Ljava/lang/String;)V");
}

void jniprintf(JNIEnv* env, jobject printStream, jmethodID metId, const char* fmt, ...) {
    char buff[512];

    va_list args;
    va_start(args, fmt);
    sprintf(buff, fmt, args);
    va_end(args);

    jstring stringFromC = env->NewStringUTF(buff);
    env->CallVoidMethod(printStream, metId, stringFromC);

    env->DeleteLocalRef(stringFromC);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_mrap_loda_Loda_loadLib(
        JNIEnv* env,
        jobject /* this */, jstring jLibPath, jobject printStream) {

    jmethodID metId = getPrintStreamMetId(env, printStream);

    //jniprintf(env, printStream, metId, "inside loadLib\n");
    LOGI("inside loadLib\n");
    const char* libPath = env->GetStringUTFChars(jLibPath, (jboolean*)0);
    void* handle = dlopen(libPath, RTLD_LAZY);
    if (!handle) {
        //jniprintf(env, printStream, metId, "dlopen error\n");
        LOGI("dlopen error\n");
        return env->NewStringUTF(dlerror());
    }
    //jniprintf(env, printStream, metId, "dlopen success\n");
    LOGI("dlopen success\n");
    void (*helloPrinter)();
    helloPrinter = (void (*)())dlsym(handle, "execute");
    char* error = dlerror();
    if (error != NULL) {
        LOGI("dlsym error\n");
        return env->NewStringUTF(error);
    }
    //jniprintf(env, printStream, metId, "dlsym success\n");
    LOGI("dlsym success\n");
    helloPrinter();
    dlclose(handle);
    env->ReleaseStringUTFChars(jLibPath, libPath);
    //jniprintf(env, printStream, metId, "handle closed\n");
    LOGI("handle closed\n");
    return env->NewStringUTF("success");
}