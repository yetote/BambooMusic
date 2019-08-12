//
// Created by ether on 2019/8/6.
//

#ifndef BAMBOOMUSIC_CALLBACK_H
#define BAMBOOMUSIC_CALLBACK_H

#include <jni.h>

class Callback {
public:
    enum CALL_THREAD {
        MAIN_THREAD,
        CHILD_THREAD
    };
    JavaVM *jvm = nullptr;
    JNIEnv *env = nullptr;
    jobject jobj;
    jclass jlz;

    Callback(JNIEnv *env, _jobject *jobj);

    void callPrepare(CALL_THREAD thread, bool success, int totalTime);
    void callPlay(CALL_THREAD thread, int currentTime);

private:
    jmethodID callPrepareId;
    jmethodID callPlayingId;
};


#endif //BAMBOOMUSIC_CALLBACK_H
