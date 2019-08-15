//
// Created by ether on 2019/8/15.
//

#include "VideoPlayer.h"
#include <utility>
#include <libavutil/frame.h>

VideoPlayer::VideoPlayer(ANativeWindow *aNativeWindow, const std::string &vertexCode,
                         const std::string &fragCode, int w, int h) : width(w), height(h) {
    eglUtil = new EGLUtil{aNativeWindow};
    LOGE(VideoPlayer_TAG, "%s:初始化egl成功", __func__);
    glUtil = new GLUtil{vertexCode, fragCode};
}

void VideoPlayer::init() {
    //    @formatter:off
    vertexArr = new GLfloat[12]{
             1.0F,  1.0F,
            -1.0F,  1.0F,
            -1.0F, -1.0F,
            -1.0F, -1.0F,
             1.0F, -1.0F,
             1.0F,  1.0F,
    };
    textureArr = new GLfloat[12]{
            1.0F, 0.0F,
            0.0F, 0.0F,
            0.0F, 1.0F,
            0.0F, 1.0F,
            1.0F, 1.0F,
            1.0F, 0.0F
    };
//     colorArr = new GLfloat[18]{
//            0.0F, 0.0F, 0.0F,
//            0.0F, 0.0F, 1.0F,
//            0.0F, 1.0F, 0.0F,
//            0.0F, 1.0F, 0.0F,
//            1.0F, 1.0F, 0.0F,
//            1.0F, 1.0F, 1.0F
//    };
    //    @formatter:on

    textureIds = glUtil->createTexture();
    if (textureIds == nullptr) {
        LOGE(VideoPlayer_TAG, "创建texture数组失败");
        return;
    }

    getLocation();
    glClearColor(1.0F, 0.0F, 1.0F, 0.0F);
    glViewport(0, 0, width, height);
}

void VideoPlayer::getLocation() {
    aPosition = glGetAttribLocation(glUtil->program, "a_Position");
    aTextureCoordinates = glGetAttribLocation(glUtil->program, "a_TextureCoordinates");
    uTextureY = glGetUniformLocation(glUtil->program, "u_TextureY");
    uTextureU = glGetUniformLocation(glUtil->program, "u_TextureU");
    uTextureV = glGetUniformLocation(glUtil->program, "u_TextureV");

    uTextureArr = new GLint[3]{
            uTextureY, uTextureU, uTextureV
    };
}

void VideoPlayer::bindTexture(AVFrame *frame) {
    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureIds[i]);
        if (i == 0) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame->width, frame->height, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[i]);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame->width / 2, frame->height / 2, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[i]);
        }
        glUniform1i(uTextureArr[i], i);
    }
}

void VideoPlayer::drawFrame(AVFrame *frame) {

    LOGE(VideoPlayer_TAG, "line in 55:宽度%d,高度%d", width, height);
    glClear(GL_COLOR_BUFFER_BIT || GL_DEPTH_BUFFER_BIT);
    glUseProgram(glUtil->program);
    bindTexture(frame);

    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 0, vertexArr);
    glEnableVertexAttribArray(aPosition);
    glVertexAttribPointer(aTextureCoordinates, 2, GL_FLOAT, GL_FALSE, 0, textureArr);
    glEnableVertexAttribArray(aTextureCoordinates);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    eglSwapBuffers(eglUtil->eglDisplay, eglUtil->eglSurface);
//    av_usleep(25000);
}

