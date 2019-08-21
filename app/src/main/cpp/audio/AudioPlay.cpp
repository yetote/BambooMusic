//
// Created by ether on 2019/8/7.
//


#include <oboe/Oboe.h>
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


AudioPlay::AudioPlay(const Callback &callback1, PlayStates &playStates1) : callback(callback1),
                                                                           playStates(playStates1) {
    packet = av_packet_alloc();
    pFrame = av_frame_alloc();
    builder = new AudioStreamBuilder();
    builder->setChannelCount(ChannelCount::Stereo);
    builder->setSampleRate(48000);
    builder->setPerformanceMode(PerformanceMode::LowLatency);
    builder->setSharingMode(SharingMode::Exclusive);
    builder->setDirection(Direction::Output);
    builder->setFormat(AudioFormat::I16);
    builder->setCallback(this);
    Result result;
    builder->setCallback(this);
    result = builder->openStream(&audioStream);
    if (result != Result::OK) {
        LOGE(AudioPlay_TAG, "%s:打开流失败", __func__);
    }

    latencyTuner = new LatencyTuner(*audioStream);
    printAudioStreamInfo(audioStream);
    data = new uint8_t[MAX_AUDIO_FRAME_SIZE];
    outBuffer = static_cast<uint8_t *>(av_malloc(MAX_AUDIO_FRAME_SIZE));
    audioStream->requestStart();

    betterSize = audioStream->getBufferSizeInFrames();
}

DataCallbackResult
AudioPlay::onAudioReady(AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    int betterSize = numFrames * 4;
    if (!canPlay) {
        return DataCallbackResult::Continue;
    }
    auto buffer = static_cast<uint8_t *> (audioData);
    latencyTuner->tune();
    if (dataSize <= betterSize) {
        popData();
    }
//    if (dataSize >= betterSize) {
    for (int i = 0; i < betterSize; ++i) {
        buffer[i] = data[readPos];
        ++readPos;
        --dataSize;
        if (readPos >= MAX_AUDIO_FRAME_SIZE) {
            readPos = 0;
        }
        if (dataSize <= 0) {
            break;
        }
    }

//    }
    if ((currentTime - lastTime) >= 1) {
        callback.callPlay(callback.CHILD_THREAD, static_cast<int>(currentTime));
        lastTime = static_cast<int>(currentTime);
    }
    if (dataSize < 0 && eof) {
        LOGE(AudioPlay_TAG, "%s:数据全部播放完毕%d", __func__, dataSize);
        return DataCallbackResult::Stop;
    }
    return DataCallbackResult::Continue;
}

void AudioPlay::pushData(AVPacket *packet) {
    std::lock_guard<std::mutex> guard(codecMutex);
    canPlay = true;
    AVPacket *temp = av_packet_alloc();
    av_packet_ref(temp, packet);
    audioQueue.push(temp);
}

void AudioPlay::popData() {
//    LOGE(AudioPlay_TAG, "%sisEOF%d:", __func__, playStates.getEOF());
    std::lock_guard<std::mutex> guard(codecMutex);
    if (audioQueue.empty() && playStates.isEof()) {
        LOGE(AudioPlay_TAG, "%s:数据全部读取", __func__);
        eof = true;
        return;
    }
    if (audioQueue.empty()) {
        LOGE(AudioPlay_TAG, "%s:对列为null", __func__);
        usleep(300);
        return;
    }
    int rst;
    rst = av_packet_ref(packet, audioQueue.front());
    if (rst != 0) {
        LOGE(AudioPlay_TAG, "%s:拷贝packet失败%s", __func__, av_err2str(rst));
    }

    rst = avcodec_send_packet(pCodecCtx, packet);
    if (rst != 0) {
        LOGE(AudioPlay_TAG, "%s:发送数据出错%s", __func__, av_err2str(rst));
        audioQueue.pop();
        return;
    }
    if ((rst = avcodec_receive_frame(pCodecCtx, pFrame)) == 0) {
        auto frameCount = swr_convert(swrCtx,
                                      &outBuffer,
                                      48000 * 2,
                                      (const uint8_t **) (pFrame->data),
                                      pFrame->nb_samples);
        auto bufferSize = av_samples_get_buffer_size(nullptr, outChannelNum, frameCount,
                                                     AV_SAMPLE_FMT_S16, 1);
        currentTime = pFrame->pts * av_q2d(timeBase);
        if ((MAX_AUDIO_FRAME_SIZE - writtenPos) >= bufferSize) {
            memcpy(data + writtenPos, outBuffer, bufferSize);
            writtenPos += bufferSize;
        } else {
            auto remainingSize = MAX_AUDIO_FRAME_SIZE - writtenPos;
            memcpy(data + writtenPos, outBuffer, remainingSize);
            writtenPos = 0;
            memcpy(data + writtenPos, outBuffer + remainingSize, bufferSize - remainingSize);
            writtenPos += bufferSize - remainingSize;
        }
        dataSize += bufferSize;
    } else {
        LOGE(AudioPlay_TAG, "%s:解码出错%s", __func__, av_err2str(rst));
    }
    audioQueue.pop();
}


void AudioPlay::initSwr() {
    swrCtx = swr_alloc();
    //采样格式
    enum AVSampleFormat inSampleFmt = pCodecCtx->sample_fmt;
    enum AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;
    //采样率
    int inSampleRate = pCodecCtx->sample_rate;
    int outSampleRate = 48000;
    //声道类别
    uint64_t inSampleChannel = pCodecCtx->channel_layout;
    uint64_t outSampleChannel = AV_CH_LAYOUT_STEREO;
    //添加配置
    swr_alloc_set_opts(swrCtx,
                       outSampleChannel,
                       outSampleFmt,
                       outSampleRate,
                       inSampleChannel,
                       inSampleFmt,
                       inSampleRate,
                       0,
                       NULL);
    swr_init(swrCtx);
    outChannelNum = av_get_channel_layout_nb_channels(outSampleChannel);
}


void AudioPlay::pause() {
    audioStream->requestPause();
    LOGE(AudioPlay_TAG, "%s:暂停", __func__);
}

void AudioPlay::resume() {
    audioStream->requestStart();
    LOGE(AudioPlay_TAG, "%s:恢复", __func__);
}


AudioPlay::~AudioPlay() {

}

void AudioPlay::clear() {
    std::lock_guard<std::mutex> guard(codecMutex);
    while (!audioQueue.empty()) {
        audioQueue.pop();
    }
    dataSize = 0;
    readPos = 0;
    writtenPos = 0;
}

void AudioPlay::stop() {
    std::lock_guard<std::mutex> guard(codecMutex);
    while (!audioQueue.empty()) {
        audioQueue.pop();
    }
    LOGE(AudioPlay_TAG, "%s:队列释放完毕", __func__);
    if (audioStream != nullptr) {
        audioStream->requestStop();
        audioStream->close();
        audioStream = nullptr;
        LOGE(AudioPlay_TAG, "%s:音频流停止", __func__);
    }
    if (pCodecCtx != nullptr) {
        avcodec_free_context(&pCodecCtx);
        pCodecCtx = nullptr;
        LOGE(AudioPlay_TAG, "%s:释放codecCtx", __func__);
    }
    if (packet != nullptr) {
        av_packet_free(&packet);
        packet = nullptr;
        LOGE(AudioPlay_TAG, "%s:释放packet", __func__);
    }
    if (pFrame != nullptr) {
        av_frame_free(&pFrame);
        pFrame = nullptr;
        LOGE(AudioPlay_TAG, "%s:释放frame", __func__);
    }
    if (swrCtx != nullptr) {
        swr_free(&swrCtx);
        swrCtx = nullptr;
        LOGE(AudioPlay_TAG, "%s:释放swrctx", __func__);
    }
}

int AudioPlay::getSize() {
    std::lock_guard<std::mutex> guard(codecMutex);
    int size = audioQueue.size();
    return size;
}
