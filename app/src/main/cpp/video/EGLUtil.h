//
// Created by ether on 2019/8/15.
//

#ifndef BAMBOOMUSIC_EGLUTIL_H
#define BAMBOOMUSIC_EGLUTIL_H

#include <EGL/egl.h>
#include "../util/LogUtil.h"

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


#endif //BAMBOOMUSIC_EGLUTIL_H
