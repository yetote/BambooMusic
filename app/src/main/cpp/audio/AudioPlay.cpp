//
// Created by ether on 2019/8/7.
//


#include "AudioPlay.h"

using namespace oboe;
const size_t MAX_AUDIO_FRAME_SIZE = 48000 * 4;
static const char *audioFormatStr[] = {
        "Invalid   非法格式", // = -1,
        "Unspecified  自动格式", // = 0,
        "I16",
        "Float",
};
static const AudioFormat audioFormatEnum[] = {
        AudioFormat::Invalid,
        AudioFormat::Unspecified,
        AudioFormat::I16,
        AudioFormat::Float,
};
static const int32_t audioFormatCount = sizeof(audioFormatEnum) /
                                        sizeof(audioFormatEnum[0]);

const char *FormatToString(AudioFormat format) {
    for (int32_t i = 0; i < audioFormatCount; ++i) {
        if (audioFormatEnum[i] == format)
            return audioFormatStr[i];
    }
    return "UNKNOW_AUDIO_FORMAT";
}

const char *audioApiToString(AudioApi api) {
    switch (api) {
        case AudioApi::AAudio:
            return "AAUDIO";
        case AudioApi::OpenSLES:
            return "OpenSL";
        case AudioApi::Unspecified:
            return "Unspecified";
    }
}

void printAudioStreamInfo(AudioStream *stream) {
    LOGE(AudioPlay_TAG, "StreamID: %p", stream);

    LOGE(AudioPlay_TAG, "缓冲区容量: %d", stream->getBufferCapacityInFrames());
    LOGE(AudioPlay_TAG, "缓冲区大小: %d", stream->getBufferSizeInFrames());
    LOGE(AudioPlay_TAG, "一次读写的帧数: %d", stream->getFramesPerBurst());
    //欠载和过载在官方文档的描述里，大致是欠载-消费者消费的速度大于生产的速度，过载就是生产的速度大于消费的速度
    LOGE(AudioPlay_TAG, "欠载或过载的数量: %d", stream->getXRunCount());
    LOGE(AudioPlay_TAG, "采样率: %d", stream->getSampleRate());
    LOGE(AudioPlay_TAG, "声道布局: %d", stream->getChannelCount());
    LOGE(AudioPlay_TAG, "音频设备id: %d", stream->getDeviceId());
    LOGE(AudioPlay_TAG, "音频格式: %s", FormatToString(stream->getFormat()));
    LOGE(AudioPlay_TAG, "流的共享模式: %s", stream->getSharingMode() == SharingMode::Exclusive ?
                                      "独占" : "共享");
    LOGE(AudioPlay_TAG, "使用的音频的API：%s", audioApiToString(stream->getAudioApi()));
    PerformanceMode perfMode = stream->getPerformanceMode();
    std::string perfModeDescription;
    switch (perfMode) {
        case PerformanceMode::None:
            perfModeDescription = "默认模式";
            break;
        case PerformanceMode::LowLatency:
            perfModeDescription = "低延迟";
            break;
        case PerformanceMode::PowerSaving:
            perfModeDescription = "节能";
            break;
    }
    LOGE(AudioPlay_TAG, "性能模式: %s", perfModeDescription.c_str());


    Direction dir = stream->getDirection();
    LOGE(AudioPlay_TAG, "流方向: %s", (dir == Direction::Output ? "OUTPUT" : "INPUT"));
    if (dir == Direction::Output) {
        LOGE(AudioPlay_TAG, "输出流读取的帧数: %d", (int32_t) stream->getFramesRead());
        LOGE(AudioPlay_TAG, "输出流写入的帧数: %d", (int32_t) stream->getFramesWritten());
    } else {
        LOGE(AudioPlay_TAG, "输入流读取的帧数: %d", (int32_t) stream->getFramesRead());
        LOGE(AudioPlay_TAG, "输入流写入的帧数: %d", (int32_t) stream->getFramesWritten());
    }
}

AudioPlay::AudioPlay(std::string wpath) {
    file = fopen(wpath.c_str(), "wb+");
    builder = new AudioStreamBuilder();
    builder->setSampleRate(48000);
    Result result;
    builder->setCallback(this);
    result = builder->openStream(&audioStream);
    if (result != Result::OK) {
        LOGE(AudioPlay_TAG, "%s:打开流失败", __func__);
    }

    latencyTuner = new LatencyTuner(*audioStream);
    printAudioStreamInfo(audioStream);
    data = new uint8_t[48000 * 2 * 4];
    outBuffer = static_cast<uint8_t *>(av_malloc(MAX_AUDIO_FRAME_SIZE));
    audioStream->requestStart();

    betterSize = audioStream->getBufferSizeInFrames();

}

AudioPlay::~AudioPlay() {

}

DataCallbackResult
AudioPlay::onAudioReady(AudioStream *oboeStream, void *audioData, int32_t numFrames) {
//    LOGE(AudioPlay_TAG, "%s:回调了", __func__);
    if (!canPlay) {
        return DataCallbackResult::Continue;
    }
//    auto buffer = static_cast<uint8_t *> (audioData);
//    latencyTuner->tune();
//    if (dataSize > betterSize) {
//        LOGE(AudioPlay_TAG, "%s:播放中", __func__);
//        for (int i = 0; i < betterSize; ++i) {
//            buffer[i] = data[readPos];
//            LOGE(AudioPlay_TAG, "%s:data%d", __func__, data[readPos]);
//            readPos++;
//            if (i >= MAX_AUDIO_FRAME_SIZE) {
//                readPos = 0;
//            }
//        }
//        dataSize -= betterSize;
//    }
//    LOGE(AudioPlay_TAG, "%s:%d,%d,%d,%d", __func__, buffer[10], buffer[100], buffer[150],
//         buffer[200]);
//    if (dataSize <= betterSize * 2) {
//        LOGE(AudioPlay_TAG, "%s:数据不足，准备解码", __func__);
//        popData();
//    }
    popData();

    return DataCallbackResult::Continue;
}

void AudioPlay::pushData(AVPacket *packet) {
    canPlay = true;
    audioQueue.push(packet);
    LOGE(AudioPlay_TAG, "%s:queue's size=%d ", __func__, audioQueue.size());
}

void AudioPlay::popData() {

    memset(outBuffer, 0, MAX_AUDIO_FRAME_SIZE);
    if (audioQueue.empty()) {
//        LOGE(AudioPlay_TAG, "%s:对列为null", __func__);
        return;
    }
    AVPacket *packet = audioQueue.front();
    AVFrame *pFrame = av_frame_alloc();
    int rst = avcodec_send_packet(pCodecCtx, packet);
    if (rst != 0) {
        LOGE(AudioPlay_TAG, "%s:发送数据出错%s", __func__, av_err2str(rst));
        audioQueue.pop();
        return;
    }
    if ((rst = avcodec_receive_frame(pCodecCtx, pFrame)) == 0) {
        auto frameCount = swr_convert(swrCtx,
                                      &outBuffer,
                                      pFrame->nb_samples,
                                      (const uint8_t **) (pFrame->data),
                                      pFrame->nb_samples);
        auto bufferSize = av_samples_get_buffer_size(nullptr, outChannelNum, frameCount,
                                                     AV_SAMPLE_FMT_S16, 1);
//        LOGE(AudioPlay_TAG, "%s:解码后大小为%d", __func__, bufferSize);
//        LOGE(AudioPlay_TAG, "%s:copy前数据为%d", __func__, outBuffer[bufferSize / 2]);
        auto size = fwrite(outBuffer, 1, (size_t) bufferSize, file);
        LOGE(AudioPlay_TAG, "%s:size=%d", __func__, size);
//        if ((MAX_AUDIO_FRAME_SIZE - dataSize) >= bufferSize) {
//            memcpy(data + writtenPos, outBuffer, bufferSize);
//            LOGE(AudioPlay_TAG, "%s:copy后数据为%d", __func__,
//                 data[dataSize + writtenPos + bufferSize / 2]);
//            writtenPos += bufferSize;
//        } else {
//            auto remainingSize = MAX_AUDIO_FRAME_SIZE - dataSize;
//            memcpy(data + writtenPos, outBuffer, remainingSize);
//            writtenPos = 0;
//            memcpy(data + writtenPos, outBuffer + remainingSize, bufferSize - remainingSize);
//            writtenPos += bufferSize - remainingSize;
//        }
//        dataSize += bufferSize;
    } else {
        LOGE(AudioPlay_TAG, "%s:解码出错%d", __func__, rst);
    }
    audioQueue.pop();
}


void AudioPlay::initSwr() {
    swrCtx = swr_alloc();
    auto inSampleFmt = pCodecCtx->sample_fmt;
    auto outSampleFmt = AV_SAMPLE_FMT_S16;

    auto inChannelLayout = pCodecCtx->channel_layout;
    auto outChannelLayout = AV_CH_LAYOUT_STEREO;

    auto inSampleRate = pCodecCtx->sample_rate;
    auto outSampleRate = 44100;

    swr_alloc_set_opts(swrCtx,
                       outChannelLayout,
                       outSampleFmt,
                       outSampleRate,
                       inChannelLayout,
                       inSampleFmt,
                       inSampleRate,
                       0, nullptr);
    swr_init(swrCtx);
    outChannelNum = av_get_channel_layout_nb_channels(outChannelLayout);
}
