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

#define HardwareDecode_TAG "HardwareDecode"

class HardwareDecode {
public:
    bool checkSupport(std::string path);

    HardwareDecode(AudioPlay *audioPlay);

    virtual ~HardwareDecode();

private:
    AMediaExtractor *pVideoMediaExtractor = nullptr, *pAudioMediaExtractor = nullptr;
    AMediaCodec *pVideoCodec = nullptr, *pAudioCodec = nullptr;
    AMediaFormat *pAudioFmt = nullptr, *pVideoFmt = nullptr;
    FILE *file;
    AudioPlay *audioPlay = nullptr;

    void doDecodeWork();
};


#endif //BAMBOOMUSIC_HARDWAREDECODE_H