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

    bool getEOF() {
        return isEOF;
    }

    void setEOF(bool eof) {
        isEOF = eof;
    }

private:

    bool isEOF = false;
    bool isPause = false;
    bool isStop = false;
};


#endif //BAMBOOMUSIC_PLAYSTATES_H
