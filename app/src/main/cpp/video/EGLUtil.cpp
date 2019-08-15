//
// Created by ether on 2019/8/15.
//

#include "EGLUtil.h"

EGLUtil::EGLUtil(ANativeWindow *aNativeWindow) {
    bool rst;
    if (!initEGL(aNativeWindow)) {
        LOGE(EGLUtil_TAG, "%s:初始化egl失败", __func__);
//        return;
    }
}

bool EGLUtil::initEGL(ANativeWindow *window) {
    EGLint rst;
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == nullptr) {
        rst = eglGetError();
        LOGE(EGLUtil_TAG, "%s:获取display失败,错误码%d", __func__, rst);
        return false;
    }
    if (!eglInitialize(eglDisplay, nullptr, nullptr)) {
        LOGE(EGLUtil_TAG, "%s:初始化display失败,错误码%d", __func__, eglGetError());
        return false;
    }
    int eglConfigAttr[] = {
            EGL_BUFFER_SIZE, 32,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };
    int *numConfig = new int[1];
    EGLConfig *eglConfigArr = new EGLConfig[1];
    if (!eglChooseConfig(eglDisplay,
                         eglConfigAttr,
                         eglConfigArr,
                         sizeof(numConfig) / sizeof(numConfig[0]),
                         numConfig)) {
        LOGE(EGLUtil_TAG, "%s:egl配置失败,错误码%d", __func__, eglGetError());
        delete[] numConfig;
        delete[] eglConfigArr;
        return false;
    }
    eglConfig = eglConfigArr[0];
    delete[] numConfig;
    delete[] eglConfigArr;
    int eglContextAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    eglContext = eglCreateContext(eglDisplay, eglConfig, nullptr, eglContextAttr);
    if (eglContext == nullptr) {
        LOGE(EGLUtil_TAG, "%s:创建eglContext失败,错误码%d", __func__, eglGetError());
        return false;
    }
    int *surfaceAttr = new int[1]{EGL_NONE};
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, window, surfaceAttr);
    if (eglSurface == nullptr) {
        LOGE(EGLUtil_TAG, "%s:创建eglSurface失败,错误码%d", __func__, eglGetError());
        delete[] surfaceAttr;
        return false;
    }
    LOGE(EGLUtil_TAG,"window%p", window);
    LOGE(EGLUtil_TAG,"egldisplay%p", &eglDisplay);
    LOGE(EGLUtil_TAG,"eglConfig%p", &eglConfig);
    LOGE(EGLUtil_TAG,"eglSurface%p", &eglSurface);
    delete[] surfaceAttr;
    return true;
}
