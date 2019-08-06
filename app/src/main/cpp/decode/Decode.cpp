//
// Created by ether on 2019/8/6.
//

#include "Decode.h"


Decode::~Decode() {

}

void Decode::prepare(const std::string path) {
    av_register_all();
    avformat_network_init();
    int rst;
    pFmtCtx = avformat_alloc_context();
    rst = avformat_open_input(&pFmtCtx, path.c_str(), nullptr, nullptr);
    if (rst != 0) {
        LOGE(Decode_TAG, "%s:打开文件失败%s", __func__, av_err2str(rst));
        callback.callPrepare(callback.MAIN_THREAD, false);
        return;
    }
    rst = avformat_find_stream_info(pFmtCtx, nullptr);
    if (rst < 0) {
        LOGE(Decode_TAG, "%s:寻找流信息失败%d", __func__, rst);
        callback.callPrepare(callback.MAIN_THREAD, false);
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
        callback.callPrepare(callback.MAIN_THREAD, false);
        return;
    }
    LOGE(Decode_TAG, "%s:ffmpeg准备成功", __func__);
    callback.callPrepare(callback.MAIN_THREAD, true);
}

void Decode::play() {

}


Decode::Decode(const Callback &callback) : callback(callback) {}


