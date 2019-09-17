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
HardwareDecode::HardwareDecode(PlayStates &_playStates, const Callback &_callback) : playStates(
        _playStates), callback(_callback) {
//    @formatter:on

}

//    @formatter:off
HardwareDecode::HardwareDecode(AudioPlay *_audioPlay, PlayStates &_playStates,
                               const Callback &_callback) : audioPlay(_audioPlay),
                                                            playStates(_playStates),
                                                            callback(_callback) {
//    @formatter:on
    std::string path = "/storage/emulated/0/Android/data/com.yetote.bamboomusic/files/test.pcm";
    file = fopen(path.c_str(), "wb+");
    audioInfo = std::make_shared<MediaInfo>(MediaInfo::MEDIA_TYPE_AUDIO);
}

//    @formatter:off
HardwareDecode::HardwareDecode(AudioPlay *_audioPlay, VideoPlayer *_videoPlayer,
                               PlayStates &_playStates, const Callback &_callback) : audioPlay(
        _audioPlay), videoPlayer(_videoPlayer), playStates(_playStates), callback(_callback) {
//    @formatter:on
    audioInfo = std::make_shared<MediaInfo>(MediaInfo::MEDIA_TYPE_AUDIO);
    videoInfo = std::make_shared<MediaInfo>(MediaInfo::MEDIA_TYPE_VIDEO);

}

bool HardwareDecode::checkSupport(std::string _path) {

    media_status_t rst;
    auto pMediaExtractor = AMediaExtractor_new();
    rst = AMediaExtractor_setDataSource(pMediaExtractor, _path.c_str());
    if (rst != AMEDIA_OK) {
        LOGE(HardwareDecode_TAG, "%s:无法打开文件", __func__);
        return false;
    }
    auto trackNum = AMediaExtractor_getTrackCount(pMediaExtractor);
    for (int i = 0; i < trackNum; ++i) {
        auto *pFmt = AMediaExtractor_getTrackFormat(pMediaExtractor, i);
        LOGE(HardwareDecode_TAG, "%s:fmt=%s", __func__, AMediaFormat_toString(pFmt));
        const char *mime;
        if (!AMediaFormat_getString(pFmt, AMEDIAFORMAT_KEY_MIME, &mime)) {
            LOGE(HardwareDecode_TAG, "%s:未找到对应的解码器", __func__);
            stop();
            return false;
        } else if (strncmp(mime, "video/", 6) == 0) {
            LOGE(HardwareDecode_TAG, "%s:找到视频解码器", __func__);
            if (videoInfo) {
                videoInfo->extractor = AMediaExtractor_new();
                rst = AMediaExtractor_setDataSource(videoInfo->extractor, _path.c_str());
                if (rst != AMEDIA_OK) {
                    LOGE(HardwareDecode_TAG, "%s:初始化视频解码器失败", __func__);
                    videoInfo->isSuccess = false;
                }
                AMediaExtractor_selectTrack(videoInfo->extractor, i);
                videoInfo->codec = AMediaCodec_createDecoderByType(mime);
                //todo pwindow这时无法获取
//                AMediaCodec_configure(videoInfo->codec, pFmt, nullptr, nullptr, 0);
                videoInfo->pFmt = AMediaExtractor_getTrackFormat(videoInfo->extractor, i);
                videoInfo->isSuccess = true;
            }
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
            if (audioInfo) {
                audioInfo->extractor = AMediaExtractor_new();
                rst = AMediaExtractor_setDataSource(audioInfo->extractor, _path.c_str());
                if (rst != AMEDIA_OK) {
                    LOGE(HardwareDecode_TAG, "%s:初始化音频解码器失败", __func__);
                    audioInfo->isSuccess = false;
                }
                AMediaExtractor_selectTrack(audioInfo->extractor, i);
                audioInfo->codec = AMediaCodec_createDecoderByType(mime);
                AMediaCodec_configure(audioInfo->codec, pFmt, nullptr, nullptr, 0);
//                AMediaCodec_start(pAudioCodec);
                audioInfo->isSuccess = true;
            }
        } else {
            LOGE(HardwareDecode_TAG, "%s:非音轨或视频轨", __func__);
        }
        LOGE(HardwareDecode_TAG, "%s:释放fmt", __func__);
        AMediaFormat_delete(pFmt);
    }
    AMediaExtractor_delete(pMediaExtractor);
    playStates.setHardware(true);
    audioPlay->init(sampleRate, channelCount);
    return true;
}


void HardwareDecode::doDecodeWork(std::shared_ptr<MediaInfo> _sharedPtr) {
    int count = 0;
    if (!_sharedPtr->isSuccess) {
        return;
    }

    AMediaCodec_start(_sharedPtr->codec);

    //todo 这时需要启动codec
    while (!playStates.isStop()) {
        if (playStates.isPause()) {
            usleep(300000);
            canPause = true;
            LOGE(HardwareDecode_TAG, "%s:pause等待", __func__);
            continue;
        }
//        mutex.lock();
        if (!_sharedPtr->isInputEof) {
            auto inputIndex = AMediaCodec_dequeueInputBuffer(_sharedPtr->codec, -1);
            if (inputIndex >= 0) {
                size_t bufsize;
                auto inputBuffer = AMediaCodec_getInputBuffer(_sharedPtr->codec, inputIndex,
                                                              &bufsize);
                auto dataSize = AMediaExtractor_readSampleData(_sharedPtr->extractor, inputBuffer,
                                                               bufsize);
                if (dataSize < 0) {
                    dataSize = 0;
                    _sharedPtr->isInputEof = true;
                    LOGE(HardwareDecode_TAG, "%s:全部数据读取完毕", __func__);
                }
                auto presentationTimeUs = AMediaExtractor_getSampleTime(_sharedPtr->extractor);
                AMediaCodec_queueInputBuffer(_sharedPtr->codec, inputIndex, 0, dataSize,
                                             presentationTimeUs,
                                             _sharedPtr->isInputEof
                                             ? AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM : 0);

                AMediaExtractor_advance(_sharedPtr->extractor);
            } else {
                LOGE(HardwareDecode_TAG, "%s:放入数据失败,索引=%d", __func__, inputIndex);
//                LOGE(HardwareDecode_TAG,"%s:s",__func__);
//                mutex.unlock();
                continue;
            }
        }

        if (!_sharedPtr->isOutputEof) {
            AMediaCodecBufferInfo info;
            auto outputIndex = AMediaCodec_dequeueOutputBuffer(_sharedPtr->codec, &info, 0);
            if (outputIndex >= 0) {
                if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
                    LOGE(HardwareDecode_TAG, "%s:解码数据全部取出", __func__);
                    _sharedPtr->isOutputEof = true;
                }

                if (_sharedPtr->type == MediaInfo::MEDIA_TYPE_AUDIO) {
                    auto readSize = info.size;
                    size_t bufSize;
                    uint8_t *buffer = AMediaCodec_getOutputBuffer(_sharedPtr->codec, outputIndex,
                                                                  &bufSize);
                    audioPlay->currentTime = (info.presentationTimeUs) / 1000000;
                    if (bufSize < 0) {
                        LOGE(HardwareDecode_TAG, "%s:未读出解码数据%d", __func__, bufSize);
                        continue;
                    }
                    uint8_t *data = new uint8_t[bufSize];
                    memcpy(data, buffer + info.offset, info.size);
//                    LOGE(HardwareDecode_TAG, "%s:size=%d", __func__, info.size);
                    while (!audioPlay->canPush(info.size)) {
                        usleep(300000);
//                        LOGE(HardwareDecode_TAG, "%s:休眠", __func__);
                    }
//                    LOGE(HardwareDecode_TAG, "%s:开始填充数据", __func__);
                    audioPlay->pushData(data, info.size);
//                    LOGE(HardwareDecode_TAG, "%s:数据填充完成", __func__);
                    delete[] data;
                } else {
                    int64_t presentationNano = info.presentationTimeUs * 1000;
                    if (_sharedPtr->renderStart < 0) {
                        _sharedPtr->renderStart = systemnanotime() - presentationNano;
                    }
                    int64_t delay = (_sharedPtr->renderStart + presentationNano) - systemnanotime();
                    if (delay > 0) {
                        usleep(delay / 1000);
                    }
                }
                AMediaCodec_releaseOutputBuffer(_sharedPtr->codec, outputIndex, info.size != 0);
            }
        } else {
            LOGE(HardwareDecode_TAG, "%s:取出解码数据失败", __func__);
//            mutex.unlock();
            continue;
        }
        if (_sharedPtr->isInputEof && _sharedPtr->isOutputEof) {
            LOGE(HardwareDecode_TAG, "%s:退出解码", __func__);
            playStates.setEof(true);
            _sharedPtr->isFinish = true;
//            mutex.unlock();
            break;
        }
        count++;
//        mutex.unlock();
//        LOGE(HardwareDecode_TAG, "%s:解码了%d帧", __func__, count);
    }
    _sharedPtr->isFinish = true;
    LOGE(HardwareDecode_TAG, "%s:结束解码", __func__);
}

void HardwareDecode::stop() {
    LOGE(HardwareDecode_TAG, "%s:准备释放", __func__);
    int sleepCount = 0;
    while (!isFinish) {
        if (videoInfo && videoInfo->isFinish) {
            videoInfo->destroy();
        }
        if (audioInfo->isFinish) {
            audioInfo->destroy();
        }

        if (playStates.getMediaType() == PlayStates::MEDIAO_VIDEO) {
            if (audioInfo->isFinish && videoInfo->isFinish) {
                isFinish = true;
            }
        } else {
            if (audioInfo->isFinish) {
                isFinish = true;
            }
        }

        if (sleepCount >= 100) {
            isFinish = true;
        }
        LOGE(HardwareDecode_TAG, "%s:sleepcount=%d", __func__, sleepCount);
        usleep(100000);
        ++sleepCount;
    }
    std::lock_guard<std::mutex> guard(mutex);
    if (audioPlay != nullptr) {
        audioPlay->stop();
        audioPlay = nullptr;
    }
    if (videoPlayer != nullptr) {
        videoPlayer->stop();
        videoPlayer = nullptr;
    }
//
}

void HardwareDecode::decode() {
//    std::thread decodeThread(&HardwareDecode::doDecodeWork, this);
//    decodeThread.detach();
}


void HardwareDecode::playAudio() {
    if (audioPlay != nullptr) {
        audioPlay->play();
        LOGE(HardwareDecode_TAG, "%s:audio", __func__);
        std::thread audioThread(&HardwareDecode::doDecodeWork, this, audioInfo);
        audioThread.detach();
    } else {
        LOGE(HardwareDecode_TAG, "%s:nullptr", __func__);
    }
}

void HardwareDecode::playVideo(ANativeWindow *_pWindow) {
    this->pWindow = _pWindow;
    AMediaCodec_configure(videoInfo->codec, videoInfo->pFmt, _pWindow, nullptr, 0);
    playAudio();
    std::thread videoThread(&HardwareDecode::doDecodeWork, this, videoInfo);
    videoThread.detach();
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

void HardwareDecode::seek(int progress) {
    std::lock_guard<std::mutex> guard(mutex);
    audioPlay->clear();
    while (!canPause) {
        usleep(300000);
    }
    LOGE(HardwareDecode_TAG, "%s:start seek", __func__);
    AMediaCodec_flush(audioInfo->codec);
    AMediaExtractor_seekTo(audioInfo->extractor, progress * 1000000,
                           AMEDIAEXTRACTOR_SEEK_NEXT_SYNC);

    if (videoInfo) {
        AMediaExtractor_seekTo(videoInfo->extractor, progress * 1000000,
                               AMEDIAEXTRACTOR_SEEK_NEXT_SYNC);
        AMediaCodec_flush(videoInfo->codec);
    }
    playStates.setPause(false);
    while (!audioPlay->getSize()) {
        usleep(300000);
    }
    canPause = false;
//    renderstart = -1;
//    isInputEOF = false;
//    isOutputEOF = false;
}

HardwareDecode::~HardwareDecode() {

}










