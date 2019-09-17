//
// Created by ether on 2019/8/6.
//

#ifndef BAMBOOMUSIC_CALLBACK_H
#define BAMBOOMUSIC_CALLBACK_H

#include <jni.h>
#include <string>

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

    bool callHardwareSupport(CALL_THREAD thread, std::string mutexName);

    void callHardwareCodec(CALL_THREAD thread, std::string path);

    void callPause(CALL_THREAD thread);

    void callResume(CALL_THREAD thread);

    void callSeek(CALL_THREAD thread);
    void callStop(CALL_THREAD thread);


private:
    jmethodID callPrepareId;
    jmethodID callPlayingId;
    jmethodID callPauseId;
    jmethodID callResumeId;
    jmethodID callSeekId;
    jmethodID callStopId;

    jmethodID callHardwareSupportId;
    jmethodID callHardwareCodecId;

};


#endif //BAMBOOMUSIC_CALLBACK_H
