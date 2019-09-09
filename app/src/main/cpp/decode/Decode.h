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
#include "HardwareDecode.h"
#include "FFmpegDecode.h"
#include <thread>
#include <android/native_window.h>


#define Decode_TAG "Decode"
#define LOGE(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

class Decode {
public:

    Decode(const Callback &callback, PlayStates &playStates);

    void prepare(const std::string& path);

    void playAudio();

    void
    playVideo(ANativeWindow *pWindow, int w, int h, std::string vertexCode, std::string fragCode);

    void pause();

    void resume();


    void seek(int progress);

    void stop();

    ~Decode();

    void fullScreen(int w, int h);

private:
    AudioPlay *audioPlayer = nullptr;
    VideoPlayer *videoPlayer = nullptr;
    std::mutex mutex;
    HardwareDecode *hardwareDecode = nullptr;
    FFmpegDecode *ffmpegDecode = nullptr;
    Callback callback;
    PlayStates &playStates;
};


#endif //BAMBOOMUSIC_DECODE_H
