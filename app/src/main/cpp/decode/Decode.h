//
// Created by ether on 2019/8/6.
//

#ifndef BAMBOOMUSIC_DECODE_H
#define BAMBOOMUSIC_DECODE_H

#include <string>
#include <android/log.h>
#include "../util/Callback.h"
#include "../audio/AudioPlay.h"
#include "../util/PlayStates.h"
#include <thread>

extern "C" {
#include "../includes/libavformat/avformat.h"
#include "../includes/libavcodec/avcodec.h"
};

#define Decode_TAG "Decode"
#define LOGE(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

class Decode {
public:
    Callback callback;
    PlayStates &playStates;

    Decode(const Callback &callback, PlayStates &playStates);

    void prepare(const std::string path);

    void play();

    void pause();

    void resume();

    ~Decode();

private:
    AVFormatContext *pFmtCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVStream *pStream = nullptr;
    int audioIndex = -1;
    int videoIndex = -1;
    AudioPlay *audioPlay;

    AVCodecContext *pCodecCtx;
    SwrContext *swrContext;


    int outChannelNum;
    FILE *file;
    std::string wpath;

    void free();
};


#endif //BAMBOOMUSIC_DECODE_H
