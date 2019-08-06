//
// Created by ether on 2019/8/6.
//

#ifndef BAMBOOMUSIC_DECODE_H
#define BAMBOOMUSIC_DECODE_H

#include <string>
#include <android/log.h>


extern "C" {
#include "../includes/libavformat/avformat.h"
#include "../includes/libavcodec/avcodec.h"
};

#define Decode_TAG "Decode"
#define LOGE(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

class Decode {
public:
    void prepare(const std::string path);

    void play();

    Decode();

    ~Decode();

private:
    AVFormatContext *pFmtCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVStream *pStream = nullptr;
    int audioIndex = -1;
    int videoIndex = -1;


};


#endif //BAMBOOMUSIC_DECODE_H