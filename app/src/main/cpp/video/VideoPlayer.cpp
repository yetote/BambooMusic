//
// Created by ether on 2019/8/15.
//

#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(ANativeWindow *aNativeWindow) {
    eglUtil = new EGLUtil{aNativeWindow};
    LOGE(VideoPlayer_TAG, "%s:初始化egl成功", __func__);
}
