//
// Created by ether on 2019/8/15.
//

#ifndef BAMBOOMUSIC_VIDEOPLAYER_H
#define BAMBOOMUSIC_VIDEOPLAYER_H


#include "EGLUtil.h"

#define VideoPlayer_TAG "VideoPlayer"

class VideoPlayer {
public:
    VideoPlayer(ANativeWindow *aNativeWindow);

private:
    EGLUtil *eglUtil;
};


#endif //BAMBOOMUSIC_VIDEOPLAYER_H
