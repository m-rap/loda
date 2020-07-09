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

extern "C" JNIEXPORT jstring JNICALL
Java_com_mrap_loda_Loda_loadLib(
        JNIEnv* env,
        jobject /* this */, jstring jLibPath) {
    const char* libPath = env->GetStringUTFChars(jLibPath, (jboolean*) 0);
    void* handle = dlopen(libPath, RTLD_LAZY);
    if (!handle) {
        return env->NewStringUTF(dlerror());
    }
    void (*execute)();
    execute = (void (*)())dlsym(handle, "execute");
    char* error = dlerror();
    if (error != NULL) {
        return env->NewStringUTF(error);
    }
    //execute();
    dlclose(handle);
    return env->NewStringUTF("success");
}