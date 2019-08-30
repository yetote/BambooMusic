//
// Created by ether on 2019/8/6.
//

#include <unistd.h>
#include "Decode.h"

Decode::Decode(const Callback &callback1, PlayStates &playStates1) : callback(callback1),
                                                                     playStates(playStates1) {
    audioPlayer = new AudioPlay(callback, playStates);
    videoPlayer = new VideoPlayer(callback, playStates);
    isFinish = false;
}

void Decode::prepare(const std::string path) {
    wpath = path;
    av_register_all();
    avformat_network_init();
    int rst;
    std::lock_guard<std::mutex> guard(initMutex);
    pFmtCtx = avformat_alloc_context();
    rst = avformat_open_input(&pFmtCtx, path.c_str(), nullptr, nullptr);
    if (rst != 0) {
        LOGE(Decode_TAG, "%s:打开文件失败%s", __func__, av_err2str(rst));
        callback.callPrepare(callback.CHILD_THREAD, false, 0);
        return;
    }
    LOGE(Decode_TAG, "%s:准备寻找流信息", __func__);
    rst = avformat_find_stream_info(pFmtCtx, nullptr);
    if (rst < 0) {
        LOGE(Decode_TAG, "%s:寻找流信息失败%d", __func__, rst);
        callback.callPrepare(callback.CHILD_THREAD, false, 0);
        return;
    }
    LOGE(Decode_TAG, "%s:流信息寻找完成", __func__);
    for (int i = 0; i < pFmtCtx->nb_streams; ++i) {
        if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndex = i;
        } else if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
        }
    }
    if (audioIndex == -1 && videoIndex == -1) {
        LOGE(Decode_TAG, "%s:未找到相应的流索引，请检查", __func__);
        callback.callPrepare(callback.CHILD_THREAD, false, 0);
        return;
    }
    if (audioIndex != -1) {
        pAudioStream = pFmtCtx->streams[audioIndex];
        LOGE(Decode_TAG, "%s:找到音频流", __func__);
    }
    if (videoIndex != -1) {
        pVideoStream = pFmtCtx->streams[videoIndex];
        LOGE(Decode_TAG, "%s:找到视频流", __func__);
    }
    LOGE(Decode_TAG, "%s:ffmpeg准备成功", __func__);
    audioPlayer->totalTime = pFmtCtx->duration / AV_TIME_BASE;
    LOGE(Decode_TAG, "%s:总时长%d", __func__, audioPlayer->totalTime);
    audioPlayer->timeBase = pAudioStream->time_base;
    callback.callPrepare(callback.CHILD_THREAD, true, audioPlayer->totalTime);
}

void Decode::playAudio() {
    if (audioIndex != -1 && audioPlayer != nullptr) {
        findCodec(pAudioStream, &audioPlayer->pCodecCtx, &pAudioCodec);
    }
    audioPlayer->initSwr();
    audioPlayer->play();
    decode();
}

void Decode::playVideo(ANativeWindow *pWindow, int w, int h,
                       std::string vertexCode, std::string fragCode) {
    int rst = 0;
    if (videoIndex != -1 && videoPlayer != nullptr) {
        findCodec(pVideoStream, &videoPlayer->pCodecCtx, &pVideoCodec);
        LOGE(Decode_TAG, "%s:初始化视频codec", __func__);
        videoPlayer->initEGL(vertexCode, fragCode, pWindow, w, h);
        if (videoPlayer->audioPlay == nullptr) {
            videoPlayer->audioPlay = audioPlayer;
            videoPlayer->timeBase = pVideoStream->time_base;
            int num = pVideoStream->avg_frame_rate.num;
            int den = pVideoStream->avg_frame_rate.den;
            if (num != 0 && den != 0) {
                int fps = num / den;
                LOGE(Decode_TAG, "%s:视频fps为%d", __func__, fps);
                videoPlayer->defaultSyncTime = 1.0 / fps;
            }
        }
        videoPlayer->play();
    }
    playAudio();
}

void Decode::findCodec(AVStream *pStream, AVCodecContext **avCodecContext, AVCodec **pCodec) {
    int rst;

    *pCodec = avcodec_find_decoder((pStream)->codecpar->codec_id);
    if (*pCodec == nullptr) {
        LOGE(Decode_TAG, "%s:无法打开解码器", __func__);
        return;
    }
    *avCodecContext = avcodec_alloc_context3(*pCodec);
    if (*avCodecContext == nullptr) {
        LOGE(Decode_TAG, "%s:无法分配解码器上下文", __func__);
        return;
    }
    rst = avcodec_parameters_to_context(*avCodecContext, (pStream)->codecpar);
    if (rst < 0) {
        LOGE(Decode_TAG, "%s:复制解码器上下文失败%s", __func__, av_err2str(rst));
        return;
    }
    rst = avcodec_open2(*avCodecContext, *pCodec, nullptr);
    if (rst != 0) {
        LOGE(Decode_TAG, "%s:打开解码器失败#%s", __func__, av_err2str(rst));
        return;
    }
    LOGE(Decode_TAG, "%s:解码器准备完成", __func__);
}

void Decode::pause() {
    if (audioPlayer != nullptr) {
        audioPlayer->pause();
    }
    if (videoPlayer != nullptr) {
        videoPlayer->pause();
    }

    callback.callPause(callback.MAIN_THREAD);
}

void Decode::free() {
}

void Decode::resume() {
    audioPlayer->resume();
    videoPlayer->resume();
    callback.callResume(callback.MAIN_THREAD);
}

void Decode::seek(int progress) {
    std::lock_guard<std::mutex> guard(mutex);
    auto rel = progress * AV_TIME_BASE;
    if (audioPlayer != nullptr) {
        audioPlayer->clear();
        avcodec_flush_buffers(audioPlayer->pCodecCtx);
    }
    if (videoPlayer != nullptr) {
        videoPlayer->clear();
        avcodec_flush_buffers(videoPlayer->pCodecCtx);
    }
    avformat_seek_file(pFmtCtx, -1, INT64_MIN, rel, INT64_MAX, 0);
}

void Decode::stop() {
    LOGE(Decode_TAG, "%s:申请释放%d", __func__, isFinish);
    playStates.setStop(true);
    int sleepCount = 0;
    while (!isFinish) {
        if (sleepCount >= 1000) {
            isFinish = true;
        }
        sleepCount++;
        av_usleep(1000 * 10);
        LOGE(Decode_TAG, "%s:准备释放%d", __func__, isFinish);
    }
    LOGE(Decode_TAG, "%s:开始释放", __func__);
    std::lock_guard<std::mutex> guard(initMutex);
    LOGE(Decode_TAG, "%s:准备释放audioPlayer", __func__);
    if (audioPlayer != nullptr) {
        audioPlayer->stop();
        delete audioPlayer;
        audioPlayer = nullptr;
        LOGE(Decode_TAG, "%s:audioPlayer释放完成", __func__);
    }
    LOGE(Decode_TAG, "%s:准备释放videoPlayer", __func__);
    if (videoPlayer != nullptr) {
        videoPlayer->stop();
        delete videoPlayer;
        videoPlayer = nullptr;
        LOGE(Decode_TAG, "%s:videoPlayer释放完成", __func__);
    }
    LOGE(Decode_TAG, "%s:准备释放pFmtCtx", __func__);
    if (pFmtCtx != nullptr) {
        avformat_close_input(&pFmtCtx);
        avformat_free_context(pFmtCtx);
        pFmtCtx = nullptr;
        LOGE(Decode_TAG, "%s:fmt释放完成", __func__);
    }
    callback.callStop(callback.CHILD_THREAD);
}

Decode::~Decode() {

}

void Decode::decode() {
    int rst = 0;
    AVPacket *packet = av_packet_alloc();
    while (!playStates.isEof() && !playStates.isStop()) {
        if (audioPlayer->getSize() >= 40) {
            usleep(300);
            continue;
        }
        std::lock_guard<std::mutex> guard(mutex);
        rst = av_read_frame(pFmtCtx, packet);
        LOGE(Decode_TAG, "%s:开始分包", __func__);
        if (rst < 0) {
            switch (rst) {
                case AVERROR_EOF:
                    LOGE(Decode_TAG, "%s:文件读取完毕", __func__);
                    playStates.setEof(true);
                    av_packet_free(&packet);
                    av_free(packet);
                    isFinish = true;
                    return;
                default:
                    LOGE(Decode_TAG, "%s:未知错误%s", __func__, av_err2str(rst));
                    continue;
            }
        }
        if (packet->stream_index == audioIndex) {
            if (audioPlayer != nullptr) {
                audioPlayer->pushData(packet);
                LOGE(Decode_TAG, "%s:音频入队", __func__);
                continue;
            }
        } else if (packet->stream_index == videoIndex) {
            if (videoPlayer != nullptr) {
                videoPlayer->pushData(packet);
                LOGE(Decode_TAG, "%s:视频入队", __func__);
                continue;
            }
        }
    }
    LOGE(Decode_TAG, "%s:结束分包%d", __func__, isFinish);
    isFinish = true;
}




