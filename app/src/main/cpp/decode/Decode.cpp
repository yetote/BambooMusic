//
// Created by ether on 2019/8/6.
//

#include <unistd.h>
#include "Decode.h"


Decode::~Decode() {

}

void Decode::prepare(const std::string path) {
    av_register_all();
    avformat_network_init();
    int rst;
    pFmtCtx = avformat_alloc_context();
    rst = avformat_open_input(&pFmtCtx, path.c_str(), nullptr, nullptr);
    LOGE(Decode_TAG, "%s:path:%s", __func__, path.c_str());
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
    LOGE(Decode_TAG, "%s:当前线程id%ull", __func__, std::this_thread::get_id());
    int rst;
    LOGE(Decode_TAG, "%s:准备成功，开始播放", __func__);
    if (audioIndex == -1) {
        LOGE(Decode_TAG, "%s:未找到音频索引，无法播放", __func__);
        return;
    }
    pStream = pFmtCtx->streams[audioIndex];
    pCodec = avcodec_find_decoder(pStream->codecpar->codec_id);
    if (pCodec == nullptr) {
        LOGE(Decode_TAG, "%s:无法打开解码器", __func__);
        return;
    }
    audioPlay->pCodecCtx = avcodec_alloc_context3(pCodec);
    if (audioPlay->pCodecCtx == nullptr) {
        LOGE(Decode_TAG, "%s:无法分配解码器上下文", __func__);
        return;
    }
    rst = avcodec_parameters_to_context(audioPlay->pCodecCtx, pStream->codecpar);
    if (rst < 0) {
        LOGE(Decode_TAG, "%s:复制解码器上下文失败%s", __func__, av_err2str(rst));
        return;
    }
    rst = avcodec_open2(audioPlay->pCodecCtx, pCodec, nullptr);
    if (rst != 0) {
        LOGE(Decode_TAG, "%s:打开解码器失败#%s", __func__, av_err2str(rst));
        return;
    }
    audioPlay->initSwr();
    int frameCount = 0;
    AVFrame *pFrame = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();
    while (true) {
        rst = av_read_frame(pFmtCtx, packet);
        if (rst < 0) {
            LOGE(Decode_TAG, "%s:解码失败%s", __func__, av_err2str(rst));
            break;
        }
        if (packet->stream_index != audioIndex) {
            LOGE(Decode_TAG, "%s:不是对应的轨道索引", __func__);
            continue;
        }
        audioPlay->pushData(packet);

//        rst = avcodec_send_packet(audioPlay->pCodecCtx, packet);
//        switch (rst) {
//            case AVERROR(EAGAIN):
//                LOGE(Decode_TAG, "%s:暂不接受当前输入%d", __func__, rst);
//                continue;
//            case AVERROR_EOF:
//                LOGE(Decode_TAG, "%s:解码完毕%s", __func__, av_err2str(rst));
//                return;
//            case AVERROR(EINVAL) :
//                LOGE(Decode_TAG, "%s:打开错误的编解码器%s", __func__, av_err2str(rst));
//                return;
//            case AVERROR(ENOMEM):
//                LOGE(Decode_TAG, "%s:添加输入数据%s", __func__, av_err2str(rst));
//                continue;
//            default:
//                if (rst < 0) {
//                    LOGE(Decode_TAG, "%s:未知错误%d,详情%s", __func__, rst, av_err2str(rst));
//                }
//                break;
//        }
//        while (true) {
//            sleep(1);
//            rst = avcodec_receive_frame(audioPlay->pCodecCtx, pFrame);
//            if (rst != 0) {
////                LOGE(Decode_TAG, "%s:接受帧数据失败%s", __func__, av_err2str(rst));
//                break;
//            }
//            frameCount++;
//            LOGE(Decode_TAG, "%s:解码了%d帧", __func__, frameCount);
//        }
    }
}

Decode::Decode(const Callback &callback,const std::string wpath) : callback(callback) {
    audioPlay = new AudioPlay(wpath);
}




