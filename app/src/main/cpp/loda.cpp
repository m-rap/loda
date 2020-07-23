//
// Created by Rian Prakoso on 7/14/20.
//

#include <android_native_app_glue.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <vector>
//#include <string.h>
//#include <cxxabi.h>
//#include <link.h>
//#include <stdio.h>
//#include <elf.h>
//#include <sys/types.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

using namespace std;

const char* targetFuncName = "android_main";
char finalTargetFuncName[128];

//static int callback(struct dl_phdr_info *info, size_t size, void *data) {
//    const char * libname = (const char *)data;
//    if (!strstr(info->dlpi_name, libname)) {
//        return 0;
//    }
//
//    LOGI(":::: %s, phnum %d", info->dlpi_name, info->dlpi_phnum);
//
//    for (int i = 0; i < info->dlpi_phnum; i++) {
//        //LOGI("%d p_type %d %d", i, info->dlpi_phdr[i].p_type, PT_DYNAMIC);
//        // we need to save dynamic section since it contains symbolic table
//        if (info->dlpi_phdr[i].p_type != PT_DYNAMIC) {
//            continue;
//        }
//
//        ElfW(Sym*) symtab = nullptr;
//        char* strtab = nullptr;
//        int symentries = 0;
//        int sym_cnt = 0;
//        ElfW(Word*) hash;
//
//        ElfW(Dyn*) dyn = (ElfW(Dyn*))(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
//        int dynLen = info->dlpi_phdr[i].p_memsz / sizeof(ElfW(Dyn));
//        //LOGI("p_memsz %d dynLen %d sizeof dyn %d", info->dlpi_phdr[i].p_memsz, dynLen, sizeof(ElfW(Dyn)));
//        LOGI("dlpi_addr %08x p_vaddr %08x dyn %08x", info->dlpi_addr, info->dlpi_phdr[i].p_vaddr, dyn);
//
//        for (; dyn->d_tag != DT_NULL; dyn++) {
//            //LOGI("d_tag %d. %d %d %d %d", dyn->d_tag, DT_SYMTAB, DT_STRTAB, DT_SYMENT, DT_HASH);
//            if (dyn->d_tag == DT_SYMTAB) {
//                //symtab = (ElfW(Sym*))dyn->d_un.d_ptr;
//                symtab = (ElfW(Sym*))(info->dlpi_addr + dyn->d_un.d_ptr);
//            }
//            if (dyn->d_tag == DT_STRTAB) {
//                //strtab = (char*)dyn->d_un.d_ptr;
//                strtab = (char*)(info->dlpi_addr + dyn->d_un.d_ptr);
//            }
//            if (dyn->d_tag == DT_SYMENT) {
//                symentries = dyn->d_un.d_val;
//            }
//        }
//
//        int size2 = strtab - (char *)symtab;
//        int tabLen = size2 / symentries;
//        LOGI("phdr %d symtab %08x strtab %08x symentries %d size %d tabLen %d", i, symtab, strtab, symentries, size2, tabLen);
//
//        // for each string in table
//        for (int j = 0; j < tabLen; j++) {
//            ElfW(Sym) sym = symtab[j];
//            char* str = &strtab[sym.st_name];
//            size_t demangledSize = 512;
//            int status;
//            char demangled[512];
//            abi::__cxa_demangle(str, demangled, &demangledSize, &status);
//
//            bool found = false;
//            if (status == 0) {
//                if (strstr(demangled, targetFuncName)) {
//                    found = true;
//                    strcpy(finalTargetFuncName, str);
//                }
//            } else {
//                if (strstr(str, targetFuncName)) {
//                    found = true;
//                    strcpy(finalTargetFuncName, targetFuncName);
//                }
//            }
//
//            if (found) {
//                LOGI("sym %d %08x %s, %s", j, &symtab[j], str,
//                        status == 0 ? demangled : "(can't demangle)");
//            }
//        }
//        break;
//    }
//    return 0;
//}

void trimStr(char* str) {
    if (str[0] == '\0')
        return;

    char sz[4096];
    char* temp = sz;
    strcpy(temp, str);

    while (isspace(*temp))
        temp++;

    char* temp2 = temp + strlen(temp);

    if (temp2 != temp)
    {
        while (isspace(*--temp2));
        *(temp2 + 1)='\0';
    }
    strcpy(str, temp);
}

int splitString(const char* input, const char* delimiter, vector< vector<char> >& output) {
    char tmpStr[512];
    strcpy(tmpStr, input);
    trimStr(tmpStr);

    char* tmpStr2 = &tmpStr[0];
    char* token = strsep(&tmpStr2, delimiter);

    while (token != NULL) {
        vector<char> el;
        el.resize(strlen(token) + 1);
        strcpy(&el[0], token);
        output.push_back(el);
        token = strsep(&tmpStr2, delimiter);
    }
    return output.size();
}

char filename[128] = "";
char targetFilename[128] = "libtarget.so";
char sourcePath[128] = "";
char targetPath[128] = "/data/data/com.mrap.loda/libtarget.so";
bool islocal = false;

void reset() {
    LOGI("reset vars");
    strcpy(finalTargetFuncName, targetFuncName);
    strcpy(filename, "");
    strcpy(targetFilename, "libtarget.so");
    strcpy(sourcePath, "");
    strcpy(targetPath, "/data/data/com.mrap.loda/libtarget.so");
    islocal = false;
}

void copyToInternal() {
    int res = mkdir("/data/data/com.mrap.loda", 0755);
    if (res != 0) {
        LOGI("mkdir failed %s", strerror(errno));
    }

    FILE* ftarget = fopen(targetPath, "wb");
    if (ftarget == NULL) {
        LOGI("cant open %s: %s", targetPath, strerror(errno));
        return;
    }
    FILE* f = fopen(sourcePath, "rb");
    if (f == NULL) {
        LOGI("cant open %s: %s", sourcePath, strerror(errno));
        fclose(ftarget);
        return;
    }

    int totalCopied = 0;
    while (true) {
        unsigned char buff[512];
        int len = fread(buff, 1, 512, f);
        if (len == 0) {
            break;
        }
        fwrite(buff, 1, len, ftarget);
        totalCopied += len;

        //unsigned char printBuff[513];
        //for (int i = 0; i < len; i++) {
        //    if (buff[i] >= 0x20 && buff[i] <= 0x7E) {
        //        printBuff[i] = buff[i];
        //    } else {
        //        printBuff[i] = '.';
        //    }
        //}
        //printBuff[len] = '\0';
        //LOGI("%s", printBuff);
    }

    LOGI("copied %d", totalCopied);

    fclose(f);
    fclose(ftarget);
}

bool getLibFunc(struct android_app* state) {
    vector<vector<char>> pathEls;
    int n = splitString(sourcePath, "/", pathEls);
    if (n > 0) {
        if (strlen(&pathEls[n - 1][0]) > 0) {
            strcpy(filename, &pathEls[n - 1][0]);
        } else if (n > 1) {
            strcpy(filename, &pathEls[n - 2][0]);
        }
    }

    if (strcmp(sourcePath, filename) == 0) {
        islocal = true;
        strcpy(targetFilename, sourcePath);
        strcpy(targetPath, sourcePath);
    }

    LOGI("%d %d sourcePath: %s, filename: %s", n, islocal, sourcePath, filename);

    if (!islocal) {
        copyToInternal();
    }

    //void* handle = dlopen(targetPath, RTLD_LAZY | RTLD_GLOBAL);
    void* handle = dlopen(targetPath, RTLD_LAZY); // RTLD_GLOBAL bikin sigsegv kalo diload kedua kali
    if (!handle) {
        LOGI("dlopen error %s", dlerror());
        return false;
    }
    LOGI("dlopen success");

    dlerror(); /* Clear any existing error */

    //dl_iterate_phdr(callback, (void*)filename);
    //no need demangle because its declared in android_native_app_glue.h as extern "C"
    //dl_iterate_phdr(callback, (void*)targetFilename);

    void (*libMainFunc)(struct android_app*);

    libMainFunc = (void (*)(struct android_app*))dlsym(handle, finalTargetFuncName);
    char* error = dlerror();
    if (error != NULL) {
        LOGI("dlsym error %s", error);
        dlclose(handle);
        return false;
    }
    LOGI("dlsym success");
    libMainFunc(state);

    LOGI("loda closing handle");
    dlclose(handle);

    return true;
}

void waitTermination(struct android_app* state) {
    while (true) {
        int ident;
        int events;
        struct android_poll_source *source;

        while ((ident = ALooper_pollAll(0, nullptr, &events,
                                        (void **) &source)) >= 0) {
            // Process this event.
            if (source != nullptr) {
                source->process(state, source);
            }
            if (state->destroyRequested)
                break;
        }
        if (state->destroyRequested)
            break;
    }
}

void android_main(struct android_app* state) {
    LOGI("loda inside android_main");

    reset();

    JNIEnv* env = state->activity->env;
    state->activity->vm->AttachCurrentThread(&env, 0);

    jobject activityObj = state->activity->clazz;
    jclass activityCls = env->GetObjectClass(activityObj);
    jmethodID methodId = env->GetMethodID(activityCls, "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jpathKey = env->NewStringUTF("libpath");
    jstring jpath = (jstring)env->CallObjectMethod(activityObj, methodId, jpathKey);
    env->DeleteLocalRef(jpathKey);
    const char* path = env->GetStringUTFChars(jpath, 0);
    strcpy(sourcePath, path);
    env->ReleaseStringUTFChars(jpath, path);

    bool res = getLibFunc(state);

    if (!res) {
        waitTermination(state);
    }

    LOGI("loda exiting android_main");
}