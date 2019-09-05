//
// Created by ether on 2019/9/5.
//

#ifndef BAMBOOMUSIC_HARDWAREDECODE_H
#define BAMBOOMUSIC_HARDWAREDECODE_H

#include <string>
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaExtractor.h"
#include "../util/LogUtil.h"
#include <thread>

#define HardwareDecode_TAG "HardwareDecode"

class HardwareDecode {
public:
    bool checkSupport(std::string path);

    HardwareDecode();

    virtual ~HardwareDecode();

private:
    AMediaExtractor *pVideoMediaExtractor = nullptr, *pAudioMediaExtractor = nullptr;
    AMediaCodec *pVideoCodec = nullptr, *pAudioCodec = nullptr;
    AMediaFormat *pAudioFmt = nullptr, *pVideoFmt = nullptr;
    FILE *file;

    void doDecodeWork();
};


#endif //BAMBOOMUSIC_HARDWAREDECODE_H
