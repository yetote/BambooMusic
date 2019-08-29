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
    callHardwareCodecId = env->GetMethodID(jlz, "callHardwareCodec", "(Ljava/lang/String;)V");
    callPauseId = env->GetMethodID(jlz, "callPause", "()V");
    callResumeId = env->GetMethodID(jlz, "callResume", "()V");
    callStopId = env->GetMethodID(jlz, "callStop", "()V");
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

void Callback::callHardwareCodec(Callback::CALL_THREAD thread, std::string path) {
    if (thread == MAIN_THREAD) {
        env->CallVoidMethod(jobj, callHardwareCodecId, path.c_str());
    } else {
        JNIEnv *jniEnv;
        if ((jvm->AttachCurrentThread(&jniEnv, nullptr)) != JNI_OK) {
            //todo 此处应该有log
        }
        jstring name = jniEnv->NewStringUTF(path.c_str());
        jniEnv->CallVoidMethod(jobj, callHardwareCodecId, name);
        jniEnv->DeleteLocalRef(name);
        jvm->DetachCurrentThread();
    }
}

void Callback::callPause(Callback::CALL_THREAD thread) {
    if (thread == MAIN_THREAD) {
        env->CallVoidMethod(jobj, callPauseId);
    } else {
        JNIEnv *jniEnv;
        if ((jvm->AttachCurrentThread(&jniEnv, nullptr)) != JNI_OK) {
            //todo 此处应该有log
        }
        jniEnv->CallVoidMethod(jobj, callPauseId);
        jvm->DetachCurrentThread();
    }
}

void Callback::callResume(Callback::CALL_THREAD thread) {
    if (thread == MAIN_THREAD) {
        env->CallVoidMethod(jobj, callResumeId);
    } else {
        JNIEnv *jniEnv;
        if ((jvm->AttachCurrentThread(&jniEnv, nullptr)) != JNI_OK) {
            //todo 此处应该有log
        }
        jniEnv->CallVoidMethod(jobj, callResumeId);
        jvm->DetachCurrentThread();
    }
}

void Callback::callStop(Callback::CALL_THREAD thread) {
    if (thread == MAIN_THREAD) {
        env->CallVoidMethod(jobj, callStopId);
    } else {
        JNIEnv *jniEnv;
        if ((jvm->AttachCurrentThread(&jniEnv, nullptr)) != JNI_OK) {
            //todo 此处应该有log
        }
        jniEnv->CallVoidMethod(jobj, callStopId);
        jvm->DetachCurrentThread();
    }
}
