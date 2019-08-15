//
// Created by ether on 2019/8/6.
//

#include <unistd.h>
#include "Decode.h"

Decode::Decode(const Callback &callback, PlayStates &playStates) : callback(callback),
                                                                   playStates(playStates) {
    audioPlayer = new AudioPlay(callback, playStates);
}

void Decode::prepare(const std::string path, ANativeWindow *pWindow, int w, int h) {
    videoPlayer = new VideoPlayer{pWindow};
//    prepare(path);
}

void Decode::prepare(const std::string path) {
    wpath = path;
    av_register_all();
    avformat_network_init();
    int rst;
    pFmtCtx = avformat_alloc_context();
    rst = avformat_open_input(&pFmtCtx, path.c_str(), nullptr, nullptr);
    LOGE(Decode_TAG, "%s:path:%s", __func__, path.c_str());
    if (rst != 0) {
        LOGE(Decode_TAG, "%s:打开文件失败%s", __func__, av_err2str(rst));
        callback.callPrepare(callback.MAIN_THREAD, false, 0);
        return;
    }
    rst = avformat_find_stream_info(pFmtCtx, nullptr);
    if (rst < 0) {
        LOGE(Decode_TAG, "%s:寻找流信息失败%d", __func__, rst);
        callback.callPrepare(callback.MAIN_THREAD, false, 0);
        return;
    }
    for (int i = 0; i < pFmtCtx->nb_streams; ++i) {
        if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndex = i;
        } else if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
        }
    }
    if (audioIndex == -1 && videoIndex == -1) {
        LOGE(Decode_TAG, "%s:未找到相应的流索引，请检查", __func__);
        callback.callPrepare(callback.MAIN_THREAD, false, 0);
        return;
    }
    pStream = pFmtCtx->streams[audioIndex];
    LOGE(Decode_TAG, "%s:ffmpeg准备成功", __func__);
    audioPlayer->totalTime = pFmtCtx->duration / AV_TIME_BASE;
    LOGE(Decode_TAG, "%s:总时长%d", __func__, audioPlayer->totalTime);
    audioPlayer->timeBase = pStream->time_base;
    callback.callPrepare(callback.MAIN_THREAD, true, audioPlayer->totalTime);
}

void Decode::play() {
    LOGE(Decode_TAG, "%s:当前线程id%ull", __func__, std::this_thread::get_id());
    int rst;
    LOGE(Decode_TAG, "%s:准备成功，开始播放", __func__);
    if (audioIndex == -1) {
        LOGE(Decode_TAG, "%s:未找到音频索引，无法播放", __func__);
        return;
    }
    pCodec = avcodec_find_decoder(pStream->codecpar->codec_id);
    if (pCodec == nullptr) {
        LOGE(Decode_TAG, "%s:无法打开解码器", __func__);
        return;
    }
    audioPlayer->pCodecCtx = avcodec_alloc_context3(pCodec);
    if (audioPlayer->pCodecCtx == nullptr) {
        LOGE(Decode_TAG, "%s:无法分配解码器上下文", __func__);
        return;
    }
    rst = avcodec_parameters_to_context(audioPlayer->pCodecCtx, pStream->codecpar);
    if (rst < 0) {
        LOGE(Decode_TAG, "%s:复制解码器上下文失败%s", __func__, av_err2str(rst));
        return;
    }
    rst = avcodec_open2(audioPlayer->pCodecCtx, pCodec, nullptr);

    if (rst != 0) {
        LOGE(Decode_TAG, "%s:打开解码器失败#%s", __func__, av_err2str(rst));
        return;
    }
//    rst = callback.callHardwareSupport(callback.CHILD_THREAD, pCodec->name);
//    if (rst != 0) {
//        callback.callHardwareCodec(callback.CHILD_THREAD, wpath);
//        audioPlayer->stop();
//        LOGE(Decode_TAG, "%s:支持硬解码", __func__);
//        return;
//    }
    audioPlayer->initSwr();
    int frameCount = 0;
    AVPacket *packet = av_packet_alloc();
    while (true) {
        rst = av_read_frame(pFmtCtx, packet);
        if (rst < 0) {
            switch (rst) {
                case AVERROR_EOF:
                    LOGE(Decode_TAG, "%s:文件读取完毕", __func__);
                    playStates.setEOF(true);
                    av_packet_free(&packet);
                    av_free(packet);
                    return;
                default:
                    LOGE(Decode_TAG, "%s:未知错误%s", __func__, av_err2str(rst));
                    continue;
            }
        }
        if (packet->stream_index != audioIndex) {
            LOGE(Decode_TAG, "%s:不是对应的轨道索引", __func__);
            continue;
        }
        audioPlayer->pushData(packet);
    }
}


void Decode::pause() {
    audioPlayer->pause();
}

void Decode::free() {
}

void Decode::resume() {
    audioPlayer->resume();
}


void Decode::seek(int progress) {
    auto rel = progress * AV_TIME_BASE;
    avformat_seek_file(pFmtCtx, -1, INT64_MIN, rel, INT64_MAX, 0);
    if (audioPlayer != nullptr) {
        audioPlayer->clear();
        avcodec_flush_buffers(audioPlayer->pCodecCtx);
    }
}


void Decode::stop() {
    if (pFmtCtx != nullptr) {
        avformat_free_context(pFmtCtx);
        pFmtCtx = nullptr;
        LOGE(Decode_TAG, "%s:释放fmtctx", __func__);
    }
    if (audioPlayer != nullptr) {
        audioPlayer->stop();
    }
}


Decode::~Decode() {

}