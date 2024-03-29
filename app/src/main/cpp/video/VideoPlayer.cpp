//
// Created by ether on 2018/10/29.
//


#include "VideoPlayer.h"

#define null NULL

GLUtil *glUtil;
EGLUtil *eglUtil;

void VideoPlayer::initVertex() {
//    @formatter:off
//    vertex = new GLfloat[24]{
////             x,     y,    s,    w
//            1.0f, 1.0f, 1.0f, 0.0f,
//            -1.0f, -1.0f, 0.0f, 0.0f,
//            -1.0f, -1.0f, 0.0f, 1.0f,
//
//            -1.0f, -1.0f, 0.0f, 1.0f,
//            1.0f, -1.0f, 1.0f, 1.0f,
//            1.0f, 1.0f, 1.0f, 0.0f
//    };
    vertexArray = new GLfloat[12]{
            1.0F, 1.0F,
            -1.0F, 1.0F,
            -1.0F, -1.0F,
            -1.0F, -1.0F,
            1.0F, -1.0F,
            1.0F, 1.0F,
    };
    textureArray = new GLfloat[12]{
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f,

            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
    };
//    textureArray = new GLfloat[18]{
//            1.0f, 0.0f, 0.0f,
//            0.0f, 1.0f, 0.0f,
//            0.0f, 0.0f, 1.0f,
//
//            1.0f, 0.0f, 0.0f,
//            1.0f, 0.0f, 1.0f,
//            1.0f, 0.0f, 0.0f
//    };
//    @formatter:on

//    delete[] vertex;
}

void VideoPlayer::initLocation(const char *vertexCode, const char *fragCode) {
    glUtil = new GLUtil(vertexCode, fragCode);
    textureArr = glUtil->createTexture();
    aPosition = glGetAttribLocation(glUtil->program, "a_Position");
    aTextureCoordinates = glGetAttribLocation(glUtil->program, "a_TextureCoordinates");
    uTexY = glGetUniformLocation(glUtil->program, "u_TexY");
    uTexU = glGetUniformLocation(glUtil->program, "u_TexU");
    uTexV = glGetUniformLocation(glUtil->program, "u_TexV");
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void
VideoPlayer::initEGL(std::string vertexCodeParam, std::string fragCodeParam,
                     ANativeWindow *windowParam, int wParam,
                     int hParam) {
    this->vertexCode = vertexCodeParam;
    this->fragCode = fragCodeParam;
    this->window = windowParam;
    this->w = wParam;
    this->h = hParam;
//    std::string path = "/storage/emulated/0/Android/data/com.yetote.bamboomusic/files/test.yuv";
//    file = fopen(path.c_str(), "wb+");

}


void VideoPlayer::draw(AVFrame *frame) {
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(glUtil->program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureArr[0]);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 frame->width,
                 frame->height,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 frame->data[0]);
    glUniform1i(uTexY, 0);
//    fwrite(frame->data[0], frame->width * frame->height, 1, file);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureArr[1]);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 frame->width / 2,
                 frame->height / 2,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 frame->data[1]);
    glUniform1i(uTexU, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureArr[2]);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 frame->width / 2,
                 frame->height / 2, 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 frame->data[2]);
    glUniform1i(uTexV, 2);

    glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 0, vertexArray);
    glEnableVertexAttribArray(aPosition);
    glVertexAttribPointer(aTextureCoordinates, 2, GL_FLOAT, GL_FALSE, 0,
                          textureArray);
    glEnableVertexAttribArray(aTextureCoordinates);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    eglSwapBuffers(eglUtil->eglDisplay, eglUtil->eglSurface);
}

void VideoPlayer::pushData(AVPacket *packet) {

    std::lock_guard<std::mutex> guard(mutex);
    AVPacket *temp = av_packet_alloc();
    int rst = av_packet_ref(temp, packet);
    videoData.push(temp);
}


void VideoPlayer::play() {
    std::thread playThread(&VideoPlayer::decode, this);
    playThread.detach();
}

VideoPlayer::VideoPlayer(const Callback &callback1, PlayStates &playStates1) : callback(callback1),
                                                                               playStates(
                                                                                       playStates1) {

}

double VideoPlayer::getVideoDiffTime(AVFrame *pFrame) {
    double pts = av_frame_get_best_effort_timestamp(pFrame);
    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
    pts *= av_q2d(timeBase);
    if (pts > 0) {
        currentTime = pts;
    }
    double diff = audioPlay->currentTime - currentTime;
    return diff;
}

double VideoPlayer::syncAV(double diff) {

    if (diff > 0.003) {
        //音频比视频快
        syncTime = syncTime * 2 / 3;
        if (syncTime < defaultSyncTime / 2) {
            //休眠时间太短了，视频播放就变得越来越快
            syncTime = defaultSyncTime * 2 / 3;
        } else if (syncTime > defaultSyncTime * 2) {
            //休眠时间太长，视频播放速度越来越慢
            syncTime = defaultSyncTime * 2;
        }
    } else if (diff < -0.003) {
        //视频比音频快
        syncTime = syncTime * 3 / 2;
        if (syncTime < defaultSyncTime / 2) {
            //休眠时间太短了，视频播放就变得越来越快
            syncTime = defaultSyncTime * 2 / 3;
        } else if (syncTime > defaultSyncTime * 2) {
            //休眠时间太长，视频播放速度越来越慢
            syncTime = defaultSyncTime * 2;
        }
    }

    if (diff >= 0.5) {
        //音频比视频快很多
        syncTime = 0;
    } else if (diff <= -0.5) {
        //视频比音频快很多
        syncTime = defaultSyncTime * 2;
    }

    if (fabs(diff) >= 10) {
        //视频不存在
        syncTime = defaultSyncTime;
    }

    return syncTime;
}

void VideoPlayer::decode() {
    eglUtil = new EGLUtil(window);
    initVertex();
    float changeH =
            (float) pCodecCtx->height / pCodecCtx->width *
            (float) w / (float) h;
    for (int i = 0; i < 12; ++i) {
        vertexArray[++i] *= changeH;
    }
    initLocation(vertexCode.c_str(), fragCode.c_str());
//    glViewport(0, 0, w, h);
    int rst;
    AVFrame *pFrame = av_frame_alloc();
    AVPacket *dataPacket = av_packet_alloc();
    while (!playStates.isStop()) {
        if (playStates.isEof() && videoData.empty()) {
            LOGE(VideoPlayer_TAG, "%s:全部文件解码完毕", __func__);
            break;
        }
//        LOGE(VideoPlayer_TAG, "%s:开始解码", __func__);
        if (playStates.isPause()) {
            usleep(300);
            continue;
        }
        if (videoData.empty()) {
            av_usleep(300);
            continue;
        }
        std::lock_guard<std::mutex> guard(mutex);
        dataPacket = videoData.front();
        videoData.pop();
        rst = avcodec_send_packet(pCodecCtx, dataPacket);
        if (rst >= 0) {
            rst = avcodec_receive_frame(pCodecCtx, pFrame);
            if (rst == AVERROR(EAGAIN)) {
                LOGE(VideoPlayer_TAG, "%s:读取解码数据失败%s", __func__, av_err2str(rst));
                continue;
            } else if (rst == AVERROR_EOF) {
                LOGE(VideoPlayer_TAG, "%s", "EOF解码完成");
                break;
            } else if (rst < 0) {
                LOGE(VideoPlayer_TAG, "%s", "解码出错");
                continue;
            } else {
                if (pFrame->format == AV_PIX_FMT_YUV420P) {
                    double diff = getVideoDiffTime(pFrame);
                    av_usleep(syncAV(diff) * 1000000);
                    draw(pFrame);
                } else {
                    AVFrame *pFrame420P = av_frame_alloc();
                    int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                                       pCodecCtx->width,
                                                       pCodecCtx->height, 1);
                    uint8_t *buffer = static_cast<uint8_t *>(av_malloc(num * sizeof(uint8_t)));
                    if (swsContext == nullptr) {
                        initSwrCtx();
                    }
                    sws_scale(swsContext,
                              pFrame->data,
                              pFrame->linesize,
                              0,
                              pFrame->height,
                              pFrame420P->data,
                              pFrame420P->linesize);
                    draw(pFrame420P);
                    av_frame_free(&pFrame420P);
                    av_free(pFrame420P);
                    av_free(buffer);
                    pFrame420P = nullptr;
                    LOGE(VideoPlayer_TAG, "yuv420p");
                }
            }
        } else {
            LOGE(VideoPlayer_TAG, "%s:send失败%s,%d", __func__, av_err2str(rst), rst);
        }
    }
    LOGE(VideoPlayer_TAG, "%s:退出解码", __func__);
    av_frame_free(&pFrame);
    av_packet_free(&dataPacket);
}

int VideoPlayer::getSize() {
    std::lock_guard<std::mutex> guard(mutex);
    int size = videoData.size();
    return size;
}

void VideoPlayer::pause() {
    playStates.setPause(true);
}

void VideoPlayer::resume() {
    playStates.setPause(false);
}

void VideoPlayer::clear() {
    std::lock_guard<std::mutex> guard(mutex);

    while (!videoData.empty()) {
        AVPacket *packet = videoData.front();
        av_packet_free(&packet);
        av_free(packet);
        videoData.pop();
    }
}

void VideoPlayer::stop() {
    std::lock_guard<std::mutex> guard(mutex);
    LOGE(VideoPlayer_TAG, "%s:开始释放", __func__);
    while (!videoData.empty()) {
        AVPacket *packet = videoData.front();
        av_packet_free(&packet);
        av_free(packet);
        videoData.pop();
    }
    LOGE(VideoPlayer_TAG, "%s:videoqueue释放完成", __func__);
    if (swsContext != nullptr) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
        LOGE(VideoPlayer_TAG, "%s:swsctx释放完成", __func__);
    }
    if (pCodecCtx != nullptr) {
        avcodec_free_context(&pCodecCtx);
        pCodecCtx = nullptr;
        LOGE(VideoPlayer_TAG, "%s:pcodecCtx释放完成", __func__);
    }
    if (glUtil != nullptr) {
        glUtil->release();
        glUtil = nullptr;
        LOGE(VideoPlayer_TAG, "%s:glutil释放完成", __func__);
    }
    if (eglUtil != nullptr) {
        eglUtil->release();
        eglUtil = nullptr;
        LOGE(VideoPlayer_TAG, "%s:eglutil释放完成", __func__);
    }
    LOGE(VideoPlayer_TAG, "%s:videoPlayer释放完成", __func__);
}

void VideoPlayer::initSwrCtx() {
    if (pCodecCtx == nullptr) {
        LOGE(VideoPlayer_TAG, "%s:初始化swrctx出错", __func__);
        return;
    }
    swsContext = sws_getContext(pCodecCtx->width,
                                pCodecCtx->height,
                                pCodecCtx->pix_fmt,
                                pCodecCtx->width,
                                pCodecCtx->height,
                                AV_PIX_FMT_YUV420P,
                                SWS_BICUBIC, nullptr, nullptr, nullptr
    );
}

void VideoPlayer::fullScreen(int w, int h) {
    this->w = w;
    this->h = h;
//    glViewport(0, 0, w, h);
}



