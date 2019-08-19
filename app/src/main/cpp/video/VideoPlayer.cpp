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
//    glUtil.(vertexCode, fragCode);
    textureArr = glUtil->createTexture();
//    LOGE("%d", textureArr[0]);
    aPosition = glGetAttribLocation(glUtil->program, "a_Position");
//    aColor = glGetAttribLocation(glUtil->program, "a_Color");
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


}


void VideoPlayer::draw(AVFrame *frame) {
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
//    av_usleep(25000);
}

void VideoPlayer::pushData(AVPacket *packet) {
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
    //todo 有疑问确定是*=？？？
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
//    eglMakeCurrent(eglUtil->eglDisplay, eglUtil->eglSurface, eglUtil->eglSurface,
//                   eglUtil->eglContext);
    initVertex();
    float changeH =
            (float) pCodecCtx->height / pCodecCtx->width *
            (float) w / (float) h;
    for (int i = 0; i < 12; ++i) {
        vertexArray[++i] *= changeH;
    }
    initLocation(vertexCode.c_str(), fragCode.c_str());
    glViewport(0, 0, w, h);
    int rst;
    AVFrame *pFrame = av_frame_alloc();
    AVPacket *dataPacket = av_packet_alloc();
    while (true) {
        if (videoData.empty()) {
            av_usleep(1000);
            continue;
        }
        dataPacket = videoData.front();
        rst = avcodec_send_packet(pCodecCtx, dataPacket);
        if (rst >= 0) {
            rst = avcodec_receive_frame(pCodecCtx, pFrame);
            if (rst == AVERROR(EAGAIN)) {
                LOGE(VideoPlayer_TAG, "%s:读取解码数据失败%s", __func__, av_err2str(rst));
                videoData.pop();
                continue;
            } else if (rst == AVERROR_EOF) {
                LOGE(VideoPlayer_TAG, "%s", "EOF解码完成");
                videoData.pop();
                break;
            } else if (rst < 0) {
                LOGE(VideoPlayer_TAG, "%s", "解码出错");
                videoData.pop();
                continue;
            } else {
                if (pFrame->format == AV_PIX_FMT_YUV420P) {
                    LOGE(VideoPlayer_TAG, "line in 109:解码成功");
                    double diff = getVideoDiffTime(pFrame);
                    LOGE(VideoPlayer_TAG, "%s:音频与视频的时间差%f", __func__, diff);
                    av_usleep(syncAV(diff) * 1000000);
                    LOGE(AudioPlay_TAG, "%s:视频线程id%ld", __func__, std::this_thread::get_id());
                    draw(pFrame);
                } else {
                    AVFrame *pFrame420P = av_frame_alloc();
                    int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                                       pCodecCtx->width,
                                                       pCodecCtx->height, 1);
                    uint8_t *buffer = static_cast<uint8_t *>(av_malloc(num * sizeof(uint8_t)));
                    av_image_fill_arrays(pFrame420P->data,
                                         pFrame420P->linesize,
                                         buffer,
                                         AV_PIX_FMT_YUV420P,
                                         pCodecCtx->width,
                                         pCodecCtx->height,
                                         1);
                    SwsContext *swsContext = sws_getContext(pCodecCtx->width,
                                                            pCodecCtx->height,
                                                            pCodecCtx->pix_fmt,
                                                            pCodecCtx->width,
                                                            pCodecCtx->height,
                                                            AV_PIX_FMT_YUV420P,
                                                            SWS_BICUBIC, nullptr, nullptr, nullptr
                    );
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
                    sws_freeContext(swsContext);
                    LOGE(VideoPlayer_TAG, "line in 144:解码成功");
                }
            }
        } else {
            LOGE(VideoPlayer_TAG, "%s:send失败%s,%d", __func__, av_err2str(rst), rst);
        }
        videoData.pop();
    }
    av_frame_free(&pFrame);
    av_packet_free(&dataPacket);
}



