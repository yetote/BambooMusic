#include <jni.h>
#include <string>
#include "decode/Decode.h"
#include "util/PlayStates.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <thread>
#define NATIVE_TAG "NATIVE"

Decode *decode;
Callback *callback;
PlayStates *playStates;
extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_prepare__Ljava_lang_String_2(JNIEnv *env, jobject thiz,
                                                                        jstring path_) {
    const char *path = env->GetStringUTFChars(path_, JNI_FALSE);
//    LOGE(NATIVE_TAG, "%s:main thread id%ull", __func__, std::this_thread::get_id());
    LOGE(NATIVE_TAG, "%s:调用音频", __func__);
    if (callback == nullptr) {
        callback = new Callback{env, thiz};
    }
    if (playStates == nullptr) {
        playStates = new PlayStates{};
    }
    if (decode == nullptr) {
        decode = new Decode{*callback, *playStates};
    }
    decode->prepare(path);
    env->ReleaseStringUTFChars(path_, path);
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
    decode->stop();
}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_yetote_bamboomusic_media_MyPlayer_prepare__Ljava_lang_String_2Landroid_view_Surface_2IILjava_lang_String_2Ljava_lang_String_2(
//        JNIEnv *env, jobject thiz, jstring path_, jobject surface, jint w, jint h,
//        jstring vertex_, jstring frag_) {
//    const char *path = env->GetStringUTFChars(path_, JNI_FALSE);
//    const char *vertex = env->GetStringUTFChars(vertex_, JNI_FALSE);
//    const char *frag = env->GetStringUTFChars(frag_, JNI_FALSE);
//
//    if (callback == nullptr) {
//        callback = new Callback{env, thiz};
//    }
//    if (playStates == nullptr) {
//        playStates = new PlayStates{};
//    }
//    if (decode == nullptr) {
//        decode = new Decode{*callback, *playStates};
//
//    }
//    LOGE(NATIVE_TAG, "%s:初始化视频", __func__);
//    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, surface);
//    decode->prepare(path, aNativeWindow, w, h, vertex, frag, *playStates);
//    env->ReleaseStringUTFChars(path_, path);
//    env->ReleaseStringUTFChars(frag_, frag);
//    env->ReleaseStringUTFChars(vertex_, vertex);
//
//}
extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_play__(JNIEnv *env, jobject thiz) {
    std::thread decodeThread(&Decode::playAudio, decode);
    decodeThread.detach();
}extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_play__Landroid_view_Surface_2IILjava_lang_String_2Ljava_lang_String_2(
        JNIEnv *env, jobject thiz, jobject surface, jint w, jint h, jstring vertex_codec,
        jstring frag_code) {

    const char *vertex = env->GetStringUTFChars(vertex_codec, JNI_FALSE);
    const char *frag = env->GetStringUTFChars(frag_code, JNI_FALSE);
    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, surface);
    std::thread decodeThread(&Decode::playVideo, decode, aNativeWindow, w, h, vertex, frag);
    decodeThread.detach();
    env->ReleaseStringUTFChars(frag_code, frag);
    env->ReleaseStringUTFChars(vertex_codec, vertex);
}