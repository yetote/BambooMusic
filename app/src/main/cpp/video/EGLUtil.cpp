//
// Created by ether on 2019/8/15.
//

#include "EGLUtil.h"

EGLUtil::EGLUtil(ANativeWindow *aNativeWindow) {
    initEGL(aNativeWindow);
}

bool EGLUtil::initEGL(ANativeWindow *window) {
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == nullptr) {
        LOGE(EGLUtil_TAG, "%s:获取display失败,错误码%d", __func__, eglGetError());
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
    delete[] surfaceAttr;
    auto rst = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    if (rst == EGL_FALSE) {
        LOGE(EGLUtil_TAG, "%s:关联egl失败,错误码%d", __func__, eglGetError());
        return false;
    }
    LOGE(EGLUtil_TAG, "%s:配置egl成功", __func__, eglGetError());
    return true;
}
