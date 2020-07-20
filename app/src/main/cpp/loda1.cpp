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
#include <vector>
#include <errno.h>
#include <elf.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#define USE_DEMANGLING 1

using namespace std;

char* targetFuncName = "android_main";

static int callback(struct dl_phdr_info *info, size_t size, void *data) {
    const char * libname = (const char *)data;
    if (!strstr(info->dlpi_name, libname)) {
        return 0;
    }

    //LOGI("loaded %s from: %s, phnum %d", libname, info->dlpi_name, info->dlpi_phnum);
    LOGI(":::: %s, phnum %d", info->dlpi_name, info->dlpi_phnum);

    for (int i = 0; i < info->dlpi_phnum; i++) {
        //LOGI("%d p_type %d %d", i, info->dlpi_phdr[i].p_type, PT_DYNAMIC);
        // we need to save dyanmic section since it contains symbolic table
        if (info->dlpi_phdr[i].p_type == PT_DYNAMIC) {
            ElfW(Sym*) symtab = nullptr;
            char* strtab = nullptr;
            int symentries = 0;
            int sym_cnt = 0;
            ElfW(Word*) hash;

            ElfW(Dyn*) dyn = (ElfW(Dyn*))(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
            int dynLen = info->dlpi_phdr[i].p_memsz / sizeof(ElfW(Dyn));
            //LOGI("p_memsz %d dynLen %d sizeof dyn %d", info->dlpi_phdr[i].p_memsz, dynLen, sizeof(ElfW(Dyn)));
            LOGI("dlpi_addr %08x p_vaddr %08x dyn %08x", info->dlpi_addr, info->dlpi_phdr[i].p_vaddr, dyn);

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
                    //symtab = (ElfW(Sym*))dyn->d_un.d_ptr;
                    symtab = (ElfW(Sym*))(info->dlpi_addr + dyn->d_un.d_ptr);
                }
                if (dyn->d_tag == DT_STRTAB) {
                    //strtab = (char*)dyn->d_un.d_ptr;
                    strtab = (char*)(info->dlpi_addr + dyn->d_un.d_ptr);
                }
                if (dyn->d_tag == DT_SYMENT) {
                    symentries = dyn->d_un.d_val;
                }
                //if (dyn->d_tag == DT_HASH) {
                //    hash = (ElfW(Word*))dyn->d_un.d_ptr;
                //    sym_cnt = hash[1];
                //}
            }

            int size2 = strtab - (char *)symtab;
            int tabLen = size2 / symentries;
            LOGI("phdr %d symtab %08x strtab %08x symentries %d size %d tabLen %d", i, symtab, strtab, symentries, size2, tabLen);

            //symtab = (ElfW(Sym*))(info->dlpi_addr + symtab);
            //strtab = (char*)(info->dlpi_addr + strtab);
            //
            //LOGI("phdr %d symtab %08x strtab %08x symentries %d size %d tabLen %d", i, symtab, strtab, symentries, size2, tabLen);

            // for each string in table
            for (int j = 0; j < tabLen; j++) {
                auto sym = symtab[j];
                auto str = &strtab[sym.st_name];
                size_t demangledSize = 512;
                int status;
                char demangled[512];
                abi::__cxa_demangle(str, demangled, &demangledSize, &status);

                bool found = false;
                if (status == 0) {
                    if (strstr(demangled, "android_main")) {
                        found = true;
                    }
                } else {
                    if (strstr(str, "android_main")) {
                        found = true;
                    }
                }

                if (found) {
                    LOGI("sym %d %08x %s, %s", j, &symtab[j], str,
                            status == 0 ? demangled : "(can't demangle)");
                }
            }
            //break;
        }
    }
    return 0;
}

bool getLibFunc(const char* path, struct android_app* state) {
    void* handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        LOGI("dlopen error");
        return false;
    }
    LOGI("dlopen success");

    link_map* linkMap = (link_map*)handle;
    //linkMap->l_addr

    dl_iterate_phdr(callback, (void*)path);

    dlclose(handle);

    LOGI("attempt reading file");

    //FILE* f = fopen("/data/app/com.mrap.loda-3KWvBqD1UHCfNkGrFUi3dA", "rb");
    //FILE* f = fopen("/lib/arm64/libtriangle.so", "rb");
    //FILE* f = fopen("/data/data/com.mrap.loda/lib/libtriangle.so", "rb");
    FILE* f = fopen(path, "rb");
    if (f != NULL) {
        //fseek(f, 0, SEEK_END);
        //size_t fSize = ftell(f);
        //fseek(f, 0, SEEK_SET);
        char buff[512];
        vector<char> printBuff;
        printBuff.reserve(512);
        while (true) {
            size_t readLen = fread(buff, 1, 512, f);
            if (readLen == 0) {
                break;
            }
            for (int i = 0; i < readLen; i++) {
                if (buff[i] > 0 && buff[i] < 128) {
                    printBuff.push_back(buff[i]);
                } else {
                    printBuff.resize(printBuff.size() + 4);
                    sprintf(&printBuff[printBuff.size() - 4], "0x%02X", buff[i]);
                }
            }
        }
        printBuff.push_back('\0');
        LOGI("file content: %s", &printBuff[0]);
        fclose(f);
    } else {
        LOGI("error opening %d %s", errno, strerror(errno));
    }

    handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);

    if (USE_DEMANGLING) {

    }

    void (*libMainFunc)(struct android_app*);

    libMainFunc = (void (*)(struct android_app*))dlsym(handle, "android_main");
    char* error = dlerror();
    if (error != NULL) {
        LOGI("dlsym error");
        dlclose(handle);
        return false;
    }
    LOGI("dlsym success");
    libMainFunc(state);

    LOGI("loda closing handle");
    dlclose(handle);

    return  true;
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

    bool res = getLibFunc(path, state);

    if (!res) {
        while (state->destroyRequested == 0);
    }

    LOGI("loda exiting android_main");
}