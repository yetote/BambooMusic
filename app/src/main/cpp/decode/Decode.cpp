//
// Created by ether on 2019/8/6.
//

#include <unistd.h>
#include "Decode.h"

//    @formatter:off
Decode::Decode(const Callback &callback1, PlayStates &playStates1) : callback(callback1),
                                                                     playStates(playStates1) {
//    @formatter:on

    audioPlayer = new AudioPlay(callback, playStates);
    if (playStates.getMediaType() == playStates.MEDIAO_VIDEO) {
        videoPlayer = new VideoPlayer(callback, playStates);
        hardwareDecode = new HardwareDecode(audioPlayer, videoPlayer, playStates, callback);
        ffmpegDecode = new FFmpegDecode(playStates, audioPlayer, videoPlayer);
    } else {
        hardwareDecode = new HardwareDecode(audioPlayer, playStates, callback);
        ffmpegDecode = new FFmpegDecode(playStates, audioPlayer);
    }
}

void Decode::prepare(const std::string &path) {
//    if (hardwareDecode->checkSupport(path)) {
//        LOGE(Decode_TAG, "%s:支持硬解", __func__);
//        callback.callPrepare(Callback::MAIN_THREAD, true, audioPlayer->totalTime);
//        LOGE(Decode_TAG, "%s:总时长=%d", __func__, audioPlayer->totalTime);
//    } else
        if (ffmpegDecode->prepare(path)) {
        playStates.setHardware(false);
        callback.callPrepare(Callback::MAIN_THREAD, true, audioPlayer->totalTime);
    } else {
        callback.callPrepare(Callback::MAIN_THREAD, false, 0);
    }
}

void Decode::playAudio() {
    if (!playStates.isHardware()) {
        ffmpegDecode->playAudio();
    } else {
        hardwareDecode->playAudio();
    }
}

//    @formatter:off
void Decode::playVideo(ANativeWindow *pWindow, int w, int h, std::string vertexCode,
                       std::string fragCode) {
//    @formatter:on
    if (!playStates.isHardware()) {
        ffmpegDecode->playVideo(pWindow, w, h, vertexCode, fragCode);
    } else {
        LOGE(Decode_TAG, "%s:pwindow=%p", __func__, pWindow);
        hardwareDecode->playVideo(pWindow);
    }
}

void Decode::pause() {
    playStates.setPause(true);
    if (!playStates.isHardware()) {
        ffmpegDecode->pause();
    } else {
        hardwareDecode->pause();
    }
    callback.callPause(Callback::CHILD_THREAD);
}


void Decode::resume() {
    playStates.setPause(false);
    if (!playStates.isHardware()) {
        ffmpegDecode->resume();
    } else {
        hardwareDecode->resume();
    }
    callback.callResume(Callback::CHILD_THREAD);
}

void Decode::seek(int progress) {
    if (!playStates.isHardware()) {
        ffmpegDecode->seek(progress);
    } else {
        hardwareDecode->seek(progress);
    }
    callback.callSeek(Callback::MAIN_THREAD);
}

void Decode::stop() {
    playStates.setStop(true);
    if (!playStates.isHardware()) {
        ffmpegDecode->stop();
        delete ffmpegDecode;
        ffmpegDecode = nullptr;
    } else {
        hardwareDecode->stop();
        delete hardwareDecode;
        hardwareDecode = nullptr;
    }
    LOGE(Decode_TAG, "%s:开始释放", __func__);
    std::lock_guard<std::mutex> guard(mutex);
    LOGE(Decode_TAG, "%s:准备释放audioPlayer", __func__);
    if (audioPlayer != nullptr) {
        delete audioPlayer;
        audioPlayer = nullptr;
        LOGE(Decode_TAG, "%s:audioPlayer释放完成", __func__);
    }
    LOGE(Decode_TAG, "%s:准备释放videoPlayer", __func__);
    if (videoPlayer != nullptr) {
        delete videoPlayer;
        videoPlayer = nullptr;
        LOGE(Decode_TAG, "%s:videoPlayer释放完成", __func__);
    }

    callback.callStop(Callback::CHILD_THREAD);
}

void Decode::fullScreen(int w, int h) {
    if (!playStates.isHardware()) {
        ffmpegDecode->fullScreen(w, h);
    }
}

Decode::~Decode() {

}




