//
// Created by ether on 2019/9/16.
//

#ifndef BAMBOOMUSIC_MEDIAINFO_H
#define BAMBOOMUSIC_MEDIAINFO_H

#include "media/NdkMediaCodec.h"
#include "media/NdkMediaExtractor.h"

class MediaInfo {
public:
    enum MEDIA_TYPE {
        MEDIA_TYPE_AUDIO,
        MEDIA_TYPE_VIDEO
    };
    MEDIA_TYPE type;
    bool isInputEof = false;
    bool isOutputEof = false;
    AMediaCodec *codec = nullptr;
    AMediaExtractor *extractor = nullptr;
    int64_t renderStart = -1;
    bool isSuccess = false;

    MediaInfo(MEDIA_TYPE type);

    virtual ~MediaInfo();
private:
};


#endif //BAMBOOMUSIC_MEDIAINFO_H
