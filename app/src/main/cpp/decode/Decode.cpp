//
// Created by ether on 2019/8/6.
//

#include <unistd.h>
#include "Decode.h"

Decode::Decode(const Callback &callback1, PlayStates &playStates1) : callback(callback1),
                                                                     playStates(playStates1) {

    audioPlayer = new AudioPlay(callback, playStates);
    if (playStates.getMediaType() == playStates.MEDIAO_VIDEO) {
        videoPlayer = new VideoPlayer(callback, playStates);
        hardwareDecode = new HardwareDecode(audioPlayer, videoPlayer, playStates, callback);
        fFmpegDecode = new FFmpegDecode(playStates, audioPlayer, videoPlayer);
    } else {
        hardwareDecode = new HardwareDecode(audioPlayer, playStates, callback);
        fFmpegDecode = new FFmpegDecode(playStates, audioPlayer);
    }
    isFinish = false;
}

void Decode::prepare(const std::string path) {
//
//    if (hardwareDecode->checkSupport(path)) {
//        LOGE(Decode_TAG, "%s:支持硬解", __func__);
//        playStates.setHardware(true);
//        callback.callPrepare(callback.MAIN_THREAD, true, audioPlayer->totalTime);
//    } else
    if (fFmpegDecode->prepare(path)) {
        playStates.setHardware(false);
        callback.callPrepare(callback.MAIN_THREAD, true, audioPlayer->totalTime);
    } else {
        callback.callPrepare(callback.CHILD_THREAD, false, 0);
    }

}

void Decode::playAudio() {
    fFmpegDecode->playAudio();
}

void Decode::playVideo(ANativeWindow *pWindow, int w, int h, std::string vertexCode,
                       std::string fragCode) {
    fFmpegDecode->playVideo(pWindow, w, h, vertexCode, fragCode);
}

void Decode::pause() {
    fFmpegDecode->pause();
    callback.callPause(callback.CHILD_THREAD);
}


void Decode::resume() {
    fFmpegDecode->resume();
    callback.callResume(callback.CHILD_THREAD);
}

void Decode::seek(int progress) {

    fFmpegDecode->seek(progress);
}

void Decode::stop() {
    playStates.setStop(true);
    fFmpegDecode->stop();
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
    callback.callStop(callback.CHILD_THREAD);
}

Decode::~Decode() {

}


void Decode::fullScreen(int w, int h) {
    fFmpegDecode->fullScreen(w, h);
}






