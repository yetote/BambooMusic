# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(PATH_PROJECT D:/AndroidStudioProjects/BambooMusic)
set(LIBS_PATH ${PATH_PROJECT}/app/libs)
set(OBOE_PATH D:/audiotest/oboe-1.2.0)

file(GLOB util_lib "util/*.cpp")

add_subdirectory(${OBOE_PATH} ./oboe)

include_directories(${OBOE_PATH}/include)
include_directories(${PATH_PROJECT}/app/src/main/cpp/includes)

add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libavutil.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${LIBS_PATH}/${ANDROID_ABI}/libswresample.so)

add_library(avcodec
        SHARED
        IMPORTED
        )

set_target_properties(
        avcodec
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libavcodec.so
)

add_library(avformat
        SHARED
        IMPORTED
        )

set_target_properties(
        avformat
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libavformat.so
)

add_library(swscale
        SHARED
        IMPORTED
        )

set_target_properties(
        swscale
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libswscale.so
)


add_library(avfilter
        SHARED
        IMPORTED
        )

set_target_properties(
        avfilter
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libavfilter.so
)

add_library(avdevice
        SHARED
        IMPORTED
        )

set_target_properties(
        avdevice
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libavdevice.so
)

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp
        Test.cpp
        decode/Decode.cpp
        util/Callback.cpp
        util/PlayStates.cpp
        util/RingArray.cpp
        audio/AudioPlay.cpp
        video/EGLUtil.cpp
        video/GLUtil.cpp
        video/VideoPlayer.cpp
        )


find_library( # Sets the name of the path variable.
        log-lib


        log)


target_link_libraries(
        # Specifies the target library.
        native-lib
        android
        EGL
        GLESv2
        oboe
        avutil
        swresample
        avformat
        avcodec
        swscale
        avfilter
        avdevice
        ${log-lib})