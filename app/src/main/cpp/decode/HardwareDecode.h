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

private:
    AMediaExtractor *pVideoMediaExtractor = nullptr, *pAudioMediaExtractor = nullptr;
    AMediaCodec *pVideoCodec = nullptr, *pAudioCodec = nullptr;
//    AMediaFormat *pAudioFmt = nullptr, *pVideoFmt = nullptr;
    AudioPlay *audioPlay = nullptr;
    VideoPlayer *videoPlayer = nullptr;
    PlayStates &playStates;
    const Callback &callback;

    void doDecodeWork();
};


#endif //BAMBOOMUSIC_HARDWAREDECODE_H
