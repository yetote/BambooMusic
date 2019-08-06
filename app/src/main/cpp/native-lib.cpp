#include <jni.h>
#include <string>
#include "decode/Decode.h"


Decode *decode;
Callback *callback;
extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_prepare(JNIEnv *env, jobject thiz, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, JNI_FALSE);
    callback = new Callback{env, thiz};
    decode=new Decode{*callback};
    decode->prepare(path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yetote_bamboomusic_media_MyPlayer_play(JNIEnv *env, jobject thiz) {
    // TODO: implement play()
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