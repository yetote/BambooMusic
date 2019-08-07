//
// Created by ether on 2019/8/7.
//

#ifndef BAMBOOMUSIC_AUDIOPLAY_H
#define BAMBOOMUSIC_AUDIOPLAY_H

extern "C" {
#include <libavcodec/avcodec.h>
};

class AudioPlay {
public:
    AudioPlay();

    ~AudioPlay();

    AVCodecContext *pCodecCtx = nullptr;
private:
};


#endif //BAMBOOMUSIC_AUDIOPLAY_H
