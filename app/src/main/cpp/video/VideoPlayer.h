//
// Created by ether on 2019/8/15.
//

#ifndef BAMBOOMUSIC_VIDEOPLAYER_H
#define BAMBOOMUSIC_VIDEOPLAYER_H


#include "EGLUtil.h"
#include "GLUtil.h"
#include "../util/Callback.h"
#include "../audio/AudioPlay.h"
#include <queue>
#include <utility>
#include <string>

extern "C" {
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
};
#define VideoPlayer_TAG "VideoPlayer"

class VideoPlayer {
public:
    GLfloat *vertex;
    GLfloat *vertexArray;
    GLfloat *textureArray;

    GLuint program;
    GLuint *textureArr;
    GLint aPosition, aColor;
    GLint aTextureCoordinates;
    GLint uTexY, uTexU, uTexV;
    AudioPlay *audioPlay = nullptr;
    double currentTime = 0;
    AVRational timeBase;

    VideoPlayer(const Callback &callback, PlayStates &playStates);

    AVCodecContext *pCodecCtx;

    void pushData(AVPacket *packet);

    void initEGL(std::string vertexCode, std::string fragCode, ANativeWindow *window, int w, int h);


    double defaultSyncTime = 0;

    void play();
    int getSize();

    void pause();

    void resume();

    void clear();
private:

    void initVertex();

    void initLocation(const char *vertexCode, const char *fragCode);

    void draw(AVFrame *frame);

    PlayStates &playStates;
    Callback callback;
    std::queue<AVPacket *> videoData;

    double getVideoDiffTime(AVFrame *pFrame);

    double syncAV(double diff);

    std::string vertexCode;
    std::string fragCode;
    ANativeWindow *window;
    int w;
    int h;
    double syncTime = 0;

    void decode();

    std::mutex mutex;
};


#endif //BAMBOOMUSIC_VIDEOPLAYER_H
