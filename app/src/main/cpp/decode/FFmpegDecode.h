//
// Created by ether on 2019/9/9.
//

#ifndef BAMBOOMUSIC_FFMPEGDECODE_H
#define BAMBOOMUSIC_FFMPEGDECODE_H

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
#define FFmpegDecode_TAG "FFmpegDecode"

class FFmpegDecode {
public:
    FFmpegDecode(PlayStates &playStates, AudioPlay *audioPlayer);

    FFmpegDecode(PlayStates &playStates, AudioPlay *audioPlayer, VideoPlayer *videoPlayer);

    bool prepare(const std::string path);

    void playAudio();

    void playVideo(ANativeWindow *pWindow, int w, int h,
                   std::string vertexCode, std::string fragCode);

    void pause();

    void resume();

    virtual ~FFmpegDecode();


    void seek(int progress);

    void stop();

    void fullScreen(int w, int h);

private:
    AVFormatContext *pFmtCtx = nullptr;
    AVCodec *pAudioCodec = nullptr;
    AVCodec *pVideoCodec = nullptr;
    AVStream *pAudioStream = nullptr;
    AVStream *pVideoStream = nullptr;
    int audioIndex = -1;
    int videoIndex = -1;
    PlayStates &playStates;
    AudioPlay *audioPlayer = nullptr;
    VideoPlayer *videoPlayer = nullptr;
    std::string wpath;
    std::mutex mutex, initMutex;
    bool isFinish;

    int findCodec(AVStream *pStream, AVCodecContext **pContext, AVCodec **pCodec);

    void decode();
};


#endif //BAMBOOMUSIC_FFMPEGDECODE_H
