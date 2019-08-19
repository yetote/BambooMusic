//
// Created by ether on 2018/10/20.
//

#ifndef BAMBOO_EGLUTIL_H
#define BAMBOO_EGLUTIL_H

#include <egl/egl.h>
#include <egl/eglext.h>
#include <android/log.h>
#include <../util/LogUtil.h>
#define EGLUtil_TAG "EGLUtil"
class EGLUtil {
public:
    EGLContext eglContext= nullptr;
    EGLDisplay eglDisplay= nullptr;
    EGLSurface eglSurface= nullptr;
    EGLConfig eglConfig= nullptr;

    EGLUtil(ANativeWindow *aNativeWindow);

private:
    bool initEGL(ANativeWindow *window);
};


#endif //BAMBOO_EGLUTIL_H
