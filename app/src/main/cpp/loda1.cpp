//
// Created by Rian Prakoso on 7/14/20.
//

#include <android_native_app_glue.h>

void android_main(struct android_app* state) {
    while (true) {
        int ident;
        int events;
        struct android_poll_source *source;

        while ((ident = ALooper_pollAll(0, nullptr, &events,
                                        (void **) &source)) >= 0) {
            // process

            if (state->destroyRequested) {
                return;
            }
        }
    }
}