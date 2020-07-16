cmake_minimum_required(VERSION 3.4.1)

add_library(sampletoload SHARED sampletoload.cpp)
target_include_directories(sampletoload PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
target_link_libraries(sampletoload android log)