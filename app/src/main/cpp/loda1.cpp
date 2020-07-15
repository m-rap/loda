//
// Created by Rian Prakoso on 7/14/20.
//

#include <android_native_app_glue.h>
#include <android/log.h>
#include <dlfcn.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

void getLibFunc(const char* path, struct android_app* state) {
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        LOGI("dlopen error\n");
        return;
    }
    LOGI("dlopen success\n");
    void (*libMainFunc)(struct android_app*);
    libMainFunc = (void (*)(struct android_app*))dlsym(handle, "android_main");
    char* error = dlerror();
    if (error != NULL) {
        LOGI("dlsym error\n");
        dlclose(handle);
        return;
    }
    LOGI("dlsym success\n");
    libMainFunc(state);
    dlclose(handle);
}

void android_main(struct android_app* state) {
    LOGI("inside android_main");

    JNIEnv* env = state->activity->env;
    state->activity->vm->AttachCurrentThread(&env, 0);

    jobject activityObj = state->activity->clazz;
    jclass activityCls = env->GetObjectClass(activityObj);
    jmethodID methodId = env->GetMethodID(activityCls, "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jpathKey = env->NewStringUTF("libpath");
    jstring jpath = (jstring)env->CallObjectMethod(activityObj, methodId, jpathKey);
    const char* path = env->GetStringUTFChars(jpath, 0);

    getLibFunc(path, state);

    env->DeleteLocalRef(jpathKey);
    env->ReleaseStringUTFChars(jpath, path);
}