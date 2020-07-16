//
// Created by Rian Prakoso on 7/10/20.
//

#include <stdio.h>
#include <android_native_app_glue.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

class HelloPrinter {
public:
    void print() {
        LOGI("Hello from HelloPrinter\n");
    }
};

extern "C" void helloPrinter() {
    HelloPrinter helloPrinter;
    helloPrinter.print();
}

extern "C" void android_main(struct android_app* state) {
    LOGI("sample main");
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

            if (state->destroyRequested) {
                LOGI("destroying loda activity");
                return;
            }
        }
    }
}