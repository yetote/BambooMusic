//
// Created by ether on 2019/8/13.
//

#ifndef BAMBOOMUSIC_PLAYSTATES_H
#define BAMBOOMUSIC_PLAYSTATES_H

#include <android/log.h>

#define LOGE(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define PlayStates_TAG "PlayStates"


class PlayStates {
public:
    PlayStates();

    bool isEof() const;

    void setEof(bool eof);

    bool isPause() const;

    void setPause(bool pause);

    bool isStop() const;

    void setStop(bool stop);

    enum MEDIA_TYPE {
        MEDIA_AUDIO,
        MEDIAO_VIDEO
    };

    MEDIA_TYPE getMediaType() const;

    void setMediaType(MEDIA_TYPE mediaType);

    bool isHardware() const;

    void setHardware(bool hardware);

private:

    bool eof = false;
    bool pause = false;
    bool stop = false;
    bool hardware = false;
    MEDIA_TYPE mediaType;
};


#endif //BAMBOOMUSIC_PLAYSTATES_H
