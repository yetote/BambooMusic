//
// Created by ether on 2019/9/5.
//

#include "HardwareDecode.h"

int64_t systemnanotime() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000000000LL + now.tv_nsec;
}

//    @formatter:off
HardwareDecode::HardwareDecode(PlayStates &playStates, const Callback &callback) : playStates(playStates),callback(callback) {
//    @formatter:on

}

//    @formatter:off
HardwareDecode::HardwareDecode(AudioPlay *audioPlay, PlayStates &playStates, const Callback &callback): audioPlay(audioPlay), playStates(playStates), callback(callback) {
//    @formatter:on

}

//    @formatter:off
HardwareDecode::HardwareDecode(AudioPlay *audioPlay, VideoPlayer *videoPlayer,PlayStates &playStates, const Callback &callback) : audioPlay(audioPlay),videoPlayer(videoPlayer),playStates(playStates),callback(callback) {
//    @formatter:on

}

bool HardwareDecode::checkSupport(std::string path) {

    media_status_t rst;
    AMediaExtractor *pMediaExtractor = AMediaExtractor_new();
    rst = AMediaExtractor_setDataSource(pMediaExtractor, path.c_str());
    if (rst != AMEDIA_OK) {
        LOGE(HardwareDecode_TAG, "%s:无法打开文件", __func__);
        return false;
    }
    auto trackNum = AMediaExtractor_getTrackCount(pMediaExtractor);
    for (int i = 0; i < trackNum; ++i) {
        AMediaFormat *pFmt = AMediaExtractor_getTrackFormat(pMediaExtractor, i);
        LOGE(HardwareDecode_TAG, "%s:fmt=%s", __func__, AMediaFormat_toString(pFmt));
        const char *mime;
        if (!AMediaFormat_getString(pFmt, AMEDIAFORMAT_KEY_MIME, &mime)) {
            LOGE(HardwareDecode_TAG, "%s:未找到对应的解码器", __func__);
            stop();
            return false;
        } else if (strncmp(mime, "video/", 6) == 0) {
            LOGE(HardwareDecode_TAG, "%s:找到视频解码器", __func__);
            pVideoMediaExtractor = pMediaExtractor;
            AMediaExtractor_selectTrack(pVideoMediaExtractor, i);
            pVideoCodec = AMediaCodec_createDecoderByType(mime);
            AMediaCodec_configure(pVideoCodec, pFmt, nullptr, nullptr, 0);
        } else if (strncmp(mime, "audio/", 6) == 0) {
            LOGE(HardwareDecode_TAG, "%s:找到音频解码器", __func__);
            int64_t totalTime = 0;
            AMediaFormat_getInt64(pFmt, "durationUs", &totalTime);
            audioPlay->totalTime = totalTime / 1000000;
            auto srst = AMediaFormat_getInt32(pFmt, "sample-rate", &sampleRate);
            if (!srst) {
                LOGE(HardwareDecode_TAG, "%s:获取采样率失败", __func__);
                sampleRate = 0;
            }
            auto crst = AMediaFormat_getInt32(pFmt, "channel-count", &channelCount);
            if (!crst) {
                LOGE(HardwareDecode_TAG, "%s:获取音频通道数失败", __func__);
                channelCount = 0;
            }

            pAudioMediaExtractor = pMediaExtractor;
            AMediaExtractor_selectTrack(pAudioMediaExtractor, i);
            pAudioCodec = AMediaCodec_createDecoderByType(mime);
            AMediaCodec_configure(pAudioCodec, pFmt, nullptr, nullptr, 0);
            AMediaCodec_start(pAudioCodec);
        } else {
            LOGE(HardwareDecode_TAG, "%s:非音轨或视频轨", __func__);
        }
        LOGE(HardwareDecode_TAG, "%s:释放fmt", __func__);
        AMediaFormat_delete(pFmt);
    }

    return true;
}


void HardwareDecode::doDecodeWork() {
    bool isInputEOF = false;
    bool isOutputEOF = false;
    int count = 0;
    while (!playStates.isStop()) {
        if (playStates.isPause()) {
            usleep(300000);
            continue;
        }
//        mutex.lock();
        if (!isInputEOF) {
            auto inputIndex = AMediaCodec_dequeueInputBuffer(pAudioCodec, 2000);
            if (inputIndex >= 0) {
                size_t bufsize;
                auto inputBuffer = AMediaCodec_getInputBuffer(pAudioCodec, inputIndex, &bufsize);
                auto dataSize = AMediaExtractor_readSampleData(pAudioMediaExtractor, inputBuffer,
                                                               bufsize);
                if (dataSize < 0) {
                    dataSize = 0;
                    isInputEOF = true;
                    LOGE(HardwareDecode_TAG, "%s:全部数据读取完毕", __func__);
                }
                auto presentationTimeUs = AMediaExtractor_getSampleTime(pAudioMediaExtractor);
                AMediaCodec_queueInputBuffer(pAudioCodec, inputIndex, 0, dataSize,
                                             presentationTimeUs,
                                             isInputEOF ? AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM
                                                        : 0);
                AMediaExtractor_advance(pAudioMediaExtractor);
            } else {
                LOGE(HardwareDecode_TAG, "%s:放入数据失败", __func__);
//                mutex.unlock();
                continue;
            }
        }

        if (!isOutputEOF) {
            AMediaCodecBufferInfo info;
            auto outputIndex = AMediaCodec_dequeueOutputBuffer(pAudioCodec, &info, 0);
            if (outputIndex >= 0) {
                if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
                    LOGE(HardwareDecode_TAG, "%s:解码数据全部取出", __func__);
                    isOutputEOF = true;
                }
                int64_t presentationNano = info.presentationTimeUs * 1000;
                if (renderstart < 0) {
                    renderstart = presentationNano;
                }
                audioPlay->currentTime = (info.presentationTimeUs) / 1000000;

                auto readSize = info.size;
                size_t bufSize;
                uint8_t *buffer = AMediaCodec_getOutputBuffer(pAudioCodec, outputIndex, &bufSize);
                if (bufSize < 0) {
                    LOGE(HardwareDecode_TAG, "%s:未读出解码数据%d", __func__, bufSize);
//                    mutex.unlock();
                    continue;
                }
                uint8_t *data = new uint8_t[bufSize];
                memcpy(data, buffer + info.offset, info.size);
                while (!audioPlay->canPush(info.size)) {
                    usleep(300000);
                    LOGE(HardwareDecode_TAG, "%s:休眠", __func__);
                }
                audioPlay->pushData(data, info.size);
                delete[] data;
                AMediaCodec_releaseOutputBuffer(pAudioCodec, outputIndex, info.size != 0);
//                mutex.unlock();
            }
        } else {
            LOGE(HardwareDecode_TAG, "%s:取出解码数据失败", __func__);
//            mutex.unlock();
            continue;
        }
        if (isInputEOF && isOutputEOF) {
            LOGE(HardwareDecode_TAG, "%s:退出解码", __func__);
            playStates.setEof(true);
            isFinish = true;
//            mutex.unlock();
            break;
        }
        count++;
//        LOGE(HardwareDecode_TAG, "%s:解码了%d帧", __func__, count);
    }
    isFinish = true;
    LOGE(HardwareDecode_TAG, "%s:结束解码", __func__);
}

void HardwareDecode::stop() {
    int sleepCount = 0;
    while (!isFinish) {
        if (sleepCount >= 100) {
            isFinish = true;
        }
        usleep(3000);
        sleepCount++;
    }
    std::lock_guard<std::mutex> guard(mutex);

    if (pAudioCodec != nullptr) {
        AMediaCodec_stop(pAudioCodec);
        AMediaCodec_delete(pAudioCodec);
        pAudioCodec = nullptr;
    }
    if (pVideoCodec != nullptr) {
        AMediaCodec_stop(pVideoCodec);
        AMediaCodec_delete(pVideoCodec);
        pVideoCodec = nullptr;
    }
    if (pAudioMediaExtractor != nullptr) {
        AMediaExtractor_delete(pAudioMediaExtractor);
        pAudioMediaExtractor = nullptr;
    }
    if (pVideoMediaExtractor != nullptr) {
        AMediaExtractor_delete(pVideoMediaExtractor);
        pVideoMediaExtractor = nullptr;
    }
    audioPlay = nullptr;
    videoPlayer = nullptr;
}

void HardwareDecode::decode() {
    std::thread decodeThread(&HardwareDecode::doDecodeWork, this);
    decodeThread.detach();
}


void HardwareDecode::playAudio() {
    if (audioPlay != nullptr) {
        audioPlay->init(sampleRate, channelCount);
        audioPlay->play();
        decode();
    }
}

void HardwareDecode::pause() {
    if (audioPlay != nullptr) {
        audioPlay->pause();
    }
}

void HardwareDecode::resume() {
    if (audioPlay != nullptr) {
        audioPlay->resume();
    }
}

HardwareDecode::~HardwareDecode() {

}







