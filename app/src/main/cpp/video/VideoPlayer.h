//
// Created by ether on 2019/8/15.
//

#ifndef BAMBOOMUSIC_VIDEOPLAYER_H
#define BAMBOOMUSIC_VIDEOPLAYER_H


#include <libavutil/frame.h>
#include "EGLUtil.h"
#include "GLUtil.h"

#define VideoPlayer_TAG "VideoPlayer"

class VideoPlayer {
public:
    VideoPlayer(ANativeWindow *aNativeWindow, const std::string &vertexCode,
                const std::string &fragCode, int w, int h);

private:
    EGLUtil *eglUtil = nullptr;
    GLUtil *glUtil = nullptr;
    int width;
    int height;

    void init();

    GLfloat *vertexArr;
    GLfloat *textureArr;
    GLuint *textureIds;

    void getLocation();

    GLint aPosition;
    GLint aTextureCoordinates;
    GLint uTextureY;
    GLint uTextureU;
    GLint uTextureV;
    GLint *uTextureArr;

    void bindTexture(AVFrame *frame);

    void drawFrame(AVFrame *frame);
};


#endif //BAMBOOMUSIC_VIDEOPLAYER_H
