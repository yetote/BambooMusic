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

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
};
#define AudioPlay_TAG "AudioPlay"
#define LOGE(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

class AudioPlay : oboe::AudioStreamCallback {
public:

    AudioPlay(std::string wpath);

    ~AudioPlay();

    AVCodecContext *pCodecCtx = nullptr;

    void pushData(AVPacket *packet);

    void initSwr();

private:
    SwrContext *swrCtx;
    std::queue<AVPacket *> audioQueue;

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;


    oboe::AudioStream *audioStream;
    oboe::AudioStreamBuilder *builder;
    oboe::LatencyTuner *latencyTuner;

    uint8_t *data;

    int outChannelNum;

    void popData();

    uint8_t *outBuffer;
    int betterSize = 0;
    //当前索引开始位置
    int readPos = 0;
    //可用的数据大小
    int dataSize = 0;
    //写入位置
    int writtenPos = 0;
    bool canPlay = false;
    FILE *file;
};


#endif //BAMBOOMUSIC_AUDIOPLAY_H
