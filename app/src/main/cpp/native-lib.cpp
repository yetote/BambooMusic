#include <jni.h>
#include <string>
#include "decode/Decode.h"

#define NATIVE_TAG "NATIVE"

Decode *decode;
Callback *callback;
extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_prepare(JNIEnv *env, jobject thiz, jstring path_,
                                                   jstring wpath_) {
    const char *path = env->GetStringUTFChars(path_, JNI_FALSE);
    const char *wpath = env->GetStringUTFChars(wpath_, JNI_FALSE);
    LOGE(NATIVE_TAG, "%s:main thread id%ull", __func__, std::this_thread::get_id());
    callback = new Callback{env, thiz};
    decode = new Decode{*callback, wpath};
    decode->prepare(path);
    env->ReleaseStringUTFChars(path_, path);
    env->ReleaseStringUTFChars(wpath_, wpath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_play(JNIEnv *env, jobject thiz) {
    // TODO: implement play()
    std::thread decodeThread(&Decode::play, decode);
    decodeThread.detach();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_pause(JNIEnv *env, jobject thiz) {
    // TODO: implement pause()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_stop(JNIEnv *env, jobject thiz) {
    // TODO: implement stop()
}