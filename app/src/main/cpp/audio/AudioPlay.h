//
// Created by ether on 2019/8/7.
//

#ifndef BAMBOOMUSIC_AUDIOPLAY_H
#define BAMBOOMUSIC_AUDIOPLAY_H

#include <oboe/AudioStream.h>
#include <oboe/LatencyTuner.h>
#include <android/log.h>
#include <queue>
#include <unistd.h>
#include <string>
#include "../util/Callback.h"
#include "../util/PlayStates.h"
#include "../util/RingArray.h"
#include <thread>
#include <oboe/Oboe.h>

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
};
#define AudioPlay_TAG "AudioPlay"
#define LOGE(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

class AudioPlay : oboe::AudioStreamCallback {
public:
    int outChannelNum;
    AVRational timeBase;
    int totalTime;
    AVCodecContext *pCodecCtx = nullptr;
    double currentTime;

    AudioPlay(const Callback &callback, PlayStates &playStates);

    void init(int32_t _sampleRate = 0, int32_t _channelCount = 0);

    void play();

    void pushData(AVPacket *packet);


    void pause();

    void resume();


    void stop();


    void clear();

    int getSize();

    ~AudioPlay();

    void pushData(uint8_t *data, size_t size);

    bool canPush(size_t size);

private:
    std::queue<AVPacket *> audioQueue;
//    std::queue<char *> hardwareAudioQueue;

    oboe::AudioStream *audioStream;
    oboe::AudioStreamBuilder *builder;
    oboe::LatencyTuner *latencyTuner;
    PlayStates &playStates;
    uint8_t *data;
    uint8_t *outBuffer;
    int betterSize = 0;
    //当前索引开始位置
    int readPos = 0;
    //可用的数据大小
    int dataSize = 0;
    //写入位置
    int writtenPos = 0;
    bool canPlay = false;
    SwrContext *swrCtx = nullptr;
    AVPacket *packet;
    AVFrame *pFrame;
    Callback callback;
    int lastTime = 0;
    bool eof;
    FILE *file;
    std::mutex codecMutex;
    RingArray<uint8_t> *ringArray = nullptr;
    RingArray<uint8_t> *hardwareArr = nullptr;
    int32_t outSampleRate;
    int32_t outChannelCount;

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

    void popData();

    void initSwr();


};


#endif //BAMBOOMUSIC_AUDIOPLAY_H
