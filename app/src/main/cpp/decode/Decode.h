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
#include "../video/VideoPlayer.h"
#include <thread>
#include <android/native_window.h>

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
    void playAudio();
    void playVideo(ANativeWindow *pWindow, int w, int h, std::string vertexCode, std::string fragCode);

    void pause();

    void resume();

    ~Decode();

    void seek(int progress);

    void stop();


private:
    AVFormatContext *pFmtCtx = nullptr;
    AVCodec *pAudioCodec = nullptr;
    AVCodec *pVideoCodec = nullptr;
    AVStream *pAudioStream = nullptr;
    AVStream *pVideoStream = nullptr;
    int audioIndex = -1;
    int videoIndex = -1;
    AudioPlay *audioPlayer = nullptr;
    VideoPlayer *videoPlayer = nullptr;
    std::string wpath;

    void free();

    void findCodec(AVStream *pStream, AVCodecContext **avCodecContext, AVCodec *pCodec);

    void decode();

};


#endif //BAMBOOMUSIC_DECODE_H
