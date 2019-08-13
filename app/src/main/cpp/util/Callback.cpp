//
// Created by ether on 2019/8/6.
//

#include "Callback.h"

Callback::Callback(JNIEnv *env, _jobject *jobject) : env(env) {
    env->GetJavaVM(&jvm);
    jobj = env->NewGlobalRef(jobject);
    jlz = env->GetObjectClass(jobj);
    callPrepareId = env->GetMethodID(jlz, "callPrepare", "(ZI)V");
    callPlayingId = env->GetMethodID(jlz, "callPlay", "(I)V");
    callHardwareSupportId = env->GetMethodID(jlz, "callHardwareSupport", "(Ljava/lang/String;)Z");
}

void Callback::callPrepare(Callback::CALL_THREAD thread, bool success, int totalTime) {
    if (thread == MAIN_THREAD) {
        env->CallVoidMethod(jobj, callPrepareId, success, totalTime);
    } else {
        JNIEnv *jniEnv;
        if ((jvm->AttachCurrentThread(&jniEnv, nullptr)) != JNI_OK) {
            //todo 此处应该有log
        }
        jniEnv->CallVoidMethod(jobj, callPrepareId, success, totalTime);
        jvm->DetachCurrentThread();
    }
}

void Callback::callPlay(Callback::CALL_THREAD thread, int currentTime) {
    if (thread == MAIN_THREAD) {
        env->CallVoidMethod(jobj, callPlayingId, currentTime);
    } else {
        JNIEnv *jniEnv;
        if ((jvm->AttachCurrentThread(&jniEnv, nullptr)) != JNI_OK) {
            //todo 此处应该有log
        }
        jniEnv->CallVoidMethod(jobj, callPlayingId, currentTime);
        jvm->DetachCurrentThread();
    }
}

bool Callback::callHardwareSupport(Callback::CALL_THREAD thread, std::string mutexName) {
    bool rst = false;
    if (thread == MAIN_THREAD) {
        rst = env->CallBooleanMethod(jobj, callHardwareSupportId, mutexName.c_str());
    } else {
        JNIEnv *jniEnv;
        if ((jvm->AttachCurrentThread(&jniEnv, nullptr)) != JNI_OK) {
            //todo 此处应该有log
        }
        jstring name = jniEnv->NewStringUTF(mutexName.c_str());
        rst = jniEnv->CallBooleanMethod(jobj, callHardwareSupportId, name);
        jniEnv->DeleteLocalRef(name);
        jvm->DetachCurrentThread();
    }
    return rst;
}
