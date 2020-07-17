//
// Created by Rian Prakoso on 7/14/20.
//

#include <android_native_app_glue.h>
#include <android/log.h>
#include <dlfcn.h>
#include <string.h>
#include <cxxabi.h>
#include <link.h>
#include <stdio.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#define USE_DEMANGLING 1

char* targetFuncName = "android_main";

static int callback(struct dl_phdr_info *info, size_t size, void *data) {
    const char * libname = (const char *)data;
    if (!strstr(info->dlpi_name, libname)) {
        return 0;
    }

    LOGI("loaded %s from: %s, phnum %d", libname, info->dlpi_name, info->dlpi_phnum);

    for (int i = 0; i < info->dlpi_phnum; i++) {
        LOGI("%d p_type %d %d", i, info->dlpi_phdr[i].p_type, PT_DYNAMIC);
        // we need to save dyanmic section since it contains symbolic table
        if (info->dlpi_phdr[i].p_type == PT_DYNAMIC) {
            ElfW(Sym*) symtab = nullptr;
            char* strtab = nullptr;
            int symentries = 0;
            int sym_cnt = 0;
            ElfW(Word*) hash;

            ElfW(Dyn*) dyn = (ElfW(Dyn*))(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
            int dynLen = info->dlpi_phdr[i].p_memsz / sizeof(ElfW(Dyn));
            LOGI("p_memsz %d dynLen %d sizeof dyn %d", info->dlpi_phdr[i].p_memsz, dynLen, sizeof(ElfW(Dyn)));

            //for (int j = 0; j < dynLen; j++) {
            //    LOGI("%d d_tag %d %d %d %d", j, dyn[j].d_tag, DT_SYMTAB, DT_STRTAB, DT_SYMENT);
            //    if (dyn[j].d_tag == DT_SYMTAB) {
            //        symtab = (Elf64_Sym *)dyn[j].d_un.d_ptr;
            //    }
            //    if (dyn[j].d_tag == DT_STRTAB) {
            //        strtab = (char*)dyn[j].d_un.d_ptr;
            //    }
            //    if (dyn[j].d_tag == DT_SYMENT) {
            //        symentries = dyn[j].d_un.d_val;
            //    }
            for (; dyn->d_tag != DT_NULL; dyn++) {
                //LOGI("d_tag %d. %d %d %d %d", dyn->d_tag, DT_SYMTAB, DT_STRTAB, DT_SYMENT, DT_HASH);
                if (dyn->d_tag == DT_SYMTAB) {
                    symtab = (ElfW(Sym*))dyn->d_un.d_ptr;
                }
                if (dyn->d_tag == DT_STRTAB) {
                    strtab = (char*)dyn->d_un.d_ptr;
                }
                if (dyn->d_tag == DT_SYMENT) {
                    symentries = dyn->d_un.d_val;
                }
                //if (dyn->d_tag == DT_HASH) {
                //    hash = (ElfW(Word*))dyn->d_un.d_ptr;
                //    sym_cnt = hash[1];
                //}
            }

            LOGI("symtab %08x strtab %08x symentries %d", (char *)symtab, strtab, symentries);
            int size2 = strtab - (char *)symtab;
            int tabLen = size2 / symentries;
            LOGI("size %d tabLen %d", size2, tabLen);

            //// for each string in table
            //for (int j = 0; j < tabLen; j++) {
            //    auto sym = symtab[j];
            //    LOGI("sym %d %08x", j, (char*)&symtab[j]);
            //    auto str = &strtab[sym.st_name];
            //    LOGI("%s", str);
            //}
            //break;
        }
    }
    return 0;
}

void getLibFunc(const char* path, struct android_app* state) {
    void* handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        LOGI("dlopen error");
        return;
    }
    LOGI("dlopen success");

    dl_iterate_phdr(callback, (void*)path);

    if (USE_DEMANGLING) {

    }

    void (*libMainFunc)(struct android_app*);

    libMainFunc = (void (*)(struct android_app*))dlsym(handle, "android_main");
    char* error = dlerror();
    if (error != NULL) {
        LOGI("dlsym error");
        dlclose(handle);
        return;
    }
    LOGI("dlsym success");
    libMainFunc(state);

    LOGI("loda closing handle");
    dlclose(handle);
}

void android_main(struct android_app* state) {
    LOGI("loda inside android_main");

    JNIEnv* env = state->activity->env;
    state->activity->vm->AttachCurrentThread(&env, 0);

    jobject activityObj = state->activity->clazz;
    jclass activityCls = env->GetObjectClass(activityObj);
    jmethodID methodId = env->GetMethodID(activityCls, "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jpathKey = env->NewStringUTF("libpath");
    jstring jpath = (jstring)env->CallObjectMethod(activityObj, methodId, jpathKey);
    env->DeleteLocalRef(jpathKey);
    const char* path = env->GetStringUTFChars(jpath, 0);
    char pathCopy[100];
    strcpy(pathCopy, path);
    env->ReleaseStringUTFChars(jpath, path);

    getLibFunc(path, state);

    LOGI("loda exiting android_main");
}