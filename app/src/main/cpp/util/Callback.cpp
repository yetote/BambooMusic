//
// Created by ether on 2019/8/6.
//

#include "Callback.h"

Callback::Callback(JNIEnv *env, _jobject *jobject) : env(env) {
    env->GetJavaVM(&jvm);
    jobj = env->NewGlobalRef(jobject);
    jlz = env->GetObjectClass(jobj);
    callPrepareId = env->GetMethodID(jlz, "callPrepare", "(Z)V");
}

void Callback::callPrepare(Callback::CALL_THREAD thread, bool success) {
    if (thread == MAIN_THREAD) {
        env->CallVoidMethod(jobj, callPrepareId, success);
    } else {
        JNIEnv *jniEnv;
        if ((jvm->AttachCurrentThread(&jniEnv, nullptr)) != JNI_OK) {
            //todo 此处应该有log
        }
        jniEnv->CallVoidMethod(jobj, callPrepareId, success);
        jvm->DetachCurrentThread();
    }
}


