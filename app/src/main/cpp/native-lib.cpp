#include <jni.h>
#include <string>
#include "decode/Decode.h"
#include "util/PlayStates.h"

#define NATIVE_TAG "NATIVE"

Decode *decode;
Callback *callback;
PlayStates *playStates;
extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_prepare(JNIEnv *env, jobject thiz, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, JNI_FALSE);
    LOGE(NATIVE_TAG, "%s:main thread id%ull", __func__, std::this_thread::get_id());
    callback = new Callback{env, thiz};
    playStates = new PlayStates{};
    decode = new Decode{*callback, *playStates};
    decode->prepare(path);
    env->ReleaseStringUTFChars(path_, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_play(JNIEnv *env, jobject thiz) {
    std::thread decodeThread(&Decode::play, decode);
    decodeThread.detach();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_pause(JNIEnv *env, jobject thiz) {
    decode->pause();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_resume(JNIEnv *env, jobject thiz) {
    decode->resume();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_seek(JNIEnv *env, jobject thiz, jint progress) {
    decode->seek(progress);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_stop(JNIEnv *env, jobject thiz) {
}