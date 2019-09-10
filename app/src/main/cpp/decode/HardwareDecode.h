//
// Created by ether on 2019/9/5.
//

#ifndef BAMBOOMUSIC_HARDWAREDECODE_H
#define BAMBOOMUSIC_HARDWAREDECODE_H

#include <string>
#include <thread>
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaExtractor.h"
#include "../util/LogUtil.h"
#include "../audio/AudioPlay.h"
#include "../video/VideoPlayer.h"

#define HardwareDecode_TAG "HardwareDecode"

class HardwareDecode {
public:
    bool checkSupport(std::string path);

    HardwareDecode(PlayStates &playStates, const Callback &callback);

    HardwareDecode(AudioPlay *audioPlay, PlayStates &playStates, const Callback &callback);

    HardwareDecode(AudioPlay *audioPlay, VideoPlayer *videoPlayer, PlayStates &playStates,
                   const Callback &callback);

    virtual ~HardwareDecode();

    void stop();

    void decode();

    void playAudio();

    void pause();

    void resume();

    void seek(int progress);

private:
    AMediaExtractor *pVideoMediaExtractor = nullptr, *pAudioMediaExtractor = nullptr;
    AMediaCodec *pVideoCodec = nullptr, *pAudioCodec = nullptr;
//    AMediaFormat *pAudioFmt = nullptr, *pVideoFmt = nullptr;
    AudioPlay *audioPlay = nullptr;
    VideoPlayer *videoPlayer = nullptr;
    PlayStates &playStates;
    const Callback &callback;
    int32_t sampleRate = 0;
    int32_t channelCount = 0;
    int64_t totalTime = 0;
    std::mutex mutex;
    bool isFinish = false;
    bool isInputEOF = false;
    bool isOutputEOF = false;
    void doDecodeWork();

    int64_t renderstart = -1;
};


#endif //BAMBOOMUSIC_HARDWAREDECODE_H
