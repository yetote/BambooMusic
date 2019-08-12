//
// Created by ether on 2019/8/6.
//

#include <unistd.h>
#include "Decode.h"


Decode::~Decode() {

}

void Decode::prepare(const std::string path) {
    file = fopen(wpath.c_str(), "wb+");
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
    uint8_t *outBuffer = new uint8_t[44100 * 4];
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
//        rst = avcodec_send_packet(audioPlay->pCodecCtx, packet);
        audioPlay->pushData(packet);
//        if (rst != 0) {
//            LOGE(Decode_TAG, "%s:发送数据失败%s", __func__, av_err2str(rst));
//            continue;
//        }
//        rst = avcodec_receive_frame(audioPlay->pCodecCtx, pFrame);
//        if (rst != 0) {
//            LOGE(Decode_TAG, "%s:接受解码后数据失败%s", __func__, av_err2str(rst));
//            continue;
//        }
//
//        int frameCount = swr_convert(audioPlay->swrCtx,
//                                     &outBuffer,
//                                     44100 * 2,
//                                     (const uint8_t **) (pFrame->data),
//                                     pFrame->nb_samples);
//        int outBufferSize = av_samples_get_buffer_size(nullptr, audioPlay->outChannelNum,
//                                                       frameCount,
//                                                       AV_SAMPLE_FMT_S16, 1);
//        auto size = fwrite(outBuffer, 1, outBufferSize, file);
//        LOGE(Decode_TAG, "%s写入了%d字节:", __func__, size);
    }
}

Decode::Decode(const Callback &callback, const std::string wpath) : callback(callback) {
    audioPlay = new AudioPlay(wpath);
    this->wpath = wpath;
}

void Decode::initSwr() {
    swrContext = swr_alloc();
    //采样格式
    auto inSampleFmt = pCodecCtx->sample_fmt;
    auto outSampleFmt = AV_SAMPLE_FMT_S16;
    //采样率
    auto inSampleRate = pCodecCtx->sample_rate;
    auto outSampleRate = 44100;
    //声道类别
    auto inSampleChannel = pCodecCtx->channel_layout;
    auto outSampleChannel = AV_CH_LAYOUT_STEREO;
    //添加配置
    swr_alloc_set_opts(swrContext,
                       outSampleChannel,
                       outSampleFmt,
                       outSampleRate,
                       inSampleChannel,
                       inSampleFmt,
                       inSampleRate,
                       0,
                       NULL);
    swr_init(swrContext);
    outChannelNum = av_get_channel_layout_nb_channels(outSampleChannel);
}




