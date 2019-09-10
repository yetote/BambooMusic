//
// Created by ether on 2019/9/9.
//

#include "FFmpegDecode.h"

//    @formatter:off
FFmpegDecode::FFmpegDecode(PlayStates &playStates, AudioPlay *audioPlayer) : playStates(playStates),audioPlayer(audioPlayer) {
//    @formatter:on

}

//    @formatter:off
FFmpegDecode::FFmpegDecode(PlayStates &playStates, AudioPlay *audioPlayer, VideoPlayer *videoPlayer): playStates(playStates), audioPlayer(audioPlayer), videoPlayer(videoPlayer) {
//    @formatter:on
}


bool FFmpegDecode::prepare(const std::string path) {
    av_register_all();
    avformat_network_init();
    int rst;
    std::lock_guard<std::mutex> guard(initMutex);
    pFmtCtx = avformat_alloc_context();
    rst = avformat_open_input(&pFmtCtx, path.c_str(), nullptr, nullptr);
    if (rst != 0) {
        LOGE(FFmpegDecode_TAG, "%s:打开文件失败%s", __func__, av_err2str(rst));
        return false;
    }
    LOGE(FFmpegDecode_TAG, "%s:准备寻找流信息", __func__);
    rst = avformat_find_stream_info(pFmtCtx, nullptr);
    if (rst < 0) {
        LOGE(FFmpegDecode_TAG, "%s:寻找流信息失败%d", __func__, rst);
        return false;
    }
    LOGE(FFmpegDecode_TAG, "%s:流信息寻找完成", __func__);
    for (int i = 0; i < pFmtCtx->nb_streams; ++i) {
        if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndex = i;
        } else if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
        }
    }
    if (audioIndex == -1 && videoIndex == -1) {
        LOGE(FFmpegDecode_TAG, "%s:未找到相应的流索引，请检查", __func__);
        return false;
    }
    if (audioIndex != -1) {
        pAudioStream = pFmtCtx->streams[audioIndex];
        LOGE(FFmpegDecode_TAG, "%s:找到音频流", __func__);
    }
    if (videoIndex != -1) {
        pVideoStream = pFmtCtx->streams[videoIndex];
        LOGE(FFmpegDecode_TAG, "%s:找到视频流", __func__);
    }
    LOGE(FFmpegDecode_TAG, "%s:ffmpeg准备成功", __func__);
    audioPlayer->totalTime = pFmtCtx->duration / AV_TIME_BASE;
    LOGE(FFmpegDecode_TAG, "%s:总时长%d", __func__, audioPlayer->totalTime);
    audioPlayer->timeBase = pAudioStream->time_base;
    return true;
}


void FFmpegDecode::playAudio() {
    int rst = 0;
    if (audioIndex != -1 && audioPlayer != nullptr) {
        rst = findCodec(pAudioStream, &audioPlayer->pCodecCtx, &pAudioCodec);
    }
    if (rst < 0) {
        isFinish = true;
        stop();
        return;
    }
    audioPlayer->init();
    audioPlayer->play();
    decode();
}

void FFmpegDecode::playVideo(ANativeWindow *pWindow, int w, int h,
                             std::string vertexCode, std::string fragCode) {
    int rst = 0;
    if (videoIndex != -1 && videoPlayer != nullptr) {
        findCodec(pVideoStream, &videoPlayer->pCodecCtx, &pVideoCodec);
        LOGE(FFmpegDecode_TAG, "%s:初始化视频codec", __func__);
        videoPlayer->initEGL(vertexCode, fragCode, pWindow, w, h);
        if (videoPlayer->audioPlay == nullptr) {
            videoPlayer->audioPlay = audioPlayer;
            videoPlayer->timeBase = pVideoStream->time_base;
            int num = pVideoStream->avg_frame_rate.num;
            int den = pVideoStream->avg_frame_rate.den;
            if (num != 0 && den != 0) {
                int fps = num / den;
                LOGE(FFmpegDecode_TAG, "%s:视频fps为%d", __func__, fps);
                videoPlayer->defaultSyncTime = 1.0 / fps;
            }
        }
        videoPlayer->play();
    }
    playAudio();
}

int FFmpegDecode::findCodec(AVStream *pStream, AVCodecContext **avCodecContext, AVCodec **pCodec) {
    int rst;

    *pCodec = avcodec_find_decoder((pStream)->codecpar->codec_id);
    if (*pCodec == nullptr) {
        LOGE(FFmpegDecode_TAG, "%s:无法打开解码器", __func__);
        return -1;
    }
    *avCodecContext = avcodec_alloc_context3(*pCodec);
    if (*avCodecContext == nullptr) {
        LOGE(FFmpegDecode_TAG, "%s:无法分配解码器上下文", __func__);
        return -1;
    }
    rst = avcodec_parameters_to_context(*avCodecContext, (pStream)->codecpar);
    if (rst < 0) {
        LOGE(FFmpegDecode_TAG, "%s:复制解码器上下文失败%s", __func__, av_err2str(rst));
        return -1;
    }
    rst = avcodec_open2(*avCodecContext, *pCodec, nullptr);
    if (rst != 0) {
        LOGE(FFmpegDecode_TAG, "%s:打开解码器失败#%s", __func__, av_err2str(rst));
        return -1;
    }
    LOGE(FFmpegDecode_TAG, "%s:解码器准备完成", __func__);
    return 0;
}


void FFmpegDecode::decode() {
    int rst = 0;
    int count = 0;
    AVPacket *packet = av_packet_alloc();
    while (!playStates.isEof() && !playStates.isStop()) {
        if (audioPlayer->getSize() > 100) {
            av_usleep(1000);
            continue;
        }
        std::lock_guard<std::mutex> guard(mutex);
        rst = av_read_frame(pFmtCtx, packet);
//        LOGE(FFmpegDecode_tag, "%s:开始分包", __func__);
        if (rst < 0) {
            switch (rst) {
                case AVERROR_EOF:
                    LOGE(FFmpegDecode_TAG, "%s:文件读取完毕", __func__);
                    playStates.setEof(true);
                    av_packet_free(&packet);
                    av_free(packet);
                    isFinish = true;
                    return;
                default:
                    LOGE(FFmpegDecode_TAG, "%s:未知错误%s", __func__, av_err2str(rst));
                    continue;
            }
        }
        if (packet->stream_index == audioIndex) {
            if (audioPlayer != nullptr) {
                audioPlayer->pushData(packet);
                continue;
            }
        } else if (packet->stream_index == videoIndex) {
            if (videoPlayer != nullptr) {
                videoPlayer->pushData(packet);
                continue;
            }
        }
    }
    LOGE(FFmpegDecode_TAG, "%s:结束分包%d", __func__, isFinish);
    isFinish = true;
}


void FFmpegDecode::pause() {
    if (audioPlayer != nullptr) {
        audioPlayer->pause();
    }
    if (videoPlayer != nullptr) {
        videoPlayer->pause();
    }
    LOGE(FFmpegDecode_TAG, "%s:暂停", __func__);
}


void FFmpegDecode::resume() {
    if (audioPlayer != nullptr) {
        audioPlayer->resume();
    }
    if (videoPlayer != nullptr) {
        videoPlayer->resume();
    }

}

void FFmpegDecode::seek(int progress) {
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

void FFmpegDecode::stop() {
    LOGE(FFmpegDecode_TAG, "%s:申请释放%d", __func__, isFinish);
    int sleepCount = 0;
    while (!isFinish) {
        if (sleepCount >= 100) {
            isFinish = true;
        }
        sleepCount++;
        av_usleep(1000 * 10);
        LOGE(FFmpegDecode_TAG, "%s:准备释放%d", __func__, isFinish);
    }
    LOGE(FFmpegDecode_TAG, "%s:开始释放", __func__);
    std::lock_guard<std::mutex> guard(initMutex);
    LOGE(FFmpegDecode_TAG, "%s:准备释放audioPlayer", __func__);
    if (audioPlayer != nullptr) {
        audioPlayer->stop();
        LOGE(FFmpegDecode_TAG, "%s:audioPlayer释放完成", __func__);
    }
    LOGE(FFmpegDecode_TAG, "%s:准备释放videoPlayer", __func__);
    if (videoPlayer != nullptr) {
        videoPlayer->stop();
        LOGE(FFmpegDecode_TAG, "%s:videoPlayer释放完成", __func__);
    }
    LOGE(FFmpegDecode_TAG, "%s:准备释放pFmtCtx", __func__);
    if (pFmtCtx != nullptr) {
        avformat_close_input(&pFmtCtx);
        avformat_free_context(pFmtCtx);
        pFmtCtx = nullptr;
        LOGE(FFmpegDecode_TAG, "%s:fmt释放完成", __func__);
    }

}

void FFmpegDecode::fullScreen(int w, int h) {
    if (videoPlayer != nullptr) {
        videoPlayer->fullScreen(w, h);
    }
}

FFmpegDecode::~FFmpegDecode() {

}




