//
// Created by ether on 2019/9/5.
//

#ifndef BAMBOOMUSIC_HARDWAREDECODE_H
#define BAMBOOMUSIC_HARDWAREDECODE_H

#include <string>
#include <thread>

#include "../util/LogUtil.h"
#include "../audio/AudioPlay.h"
#include "../video/VideoPlayer.h"
#include "../util/MediaInfo.h"

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

    void playVideo(ANativeWindow *_pWindow);

private:
    AudioPlay *audioPlay = nullptr;
    VideoPlayer *videoPlayer = nullptr;
    PlayStates &playStates;
    const Callback &callback;
    int32_t sampleRate = 0;
    int32_t channelCount = 0;
    int64_t totalTime = 0;
    std::mutex mutex;
    bool isFinish = false;
    FILE *file;
    std::shared_ptr<MediaInfo> audioInfo, videoInfo;

    void doDecodeWork(std::shared_ptr<MediaInfo>);

    ANativeWindow *pWindow = nullptr;
    int64_t renderstart = -1;
};


#endif //BAMBOOMUSIC_HARDWAREDECODE_H
