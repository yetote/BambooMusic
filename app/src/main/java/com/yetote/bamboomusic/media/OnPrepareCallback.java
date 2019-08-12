package com.yetote.bamboomusic.media;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.com.yetote.bamboomusic.media
 * @class describe
 * @time 2019/8/5 16:41
 * @change
 * @chang time
 * @class describe
 */
public interface OnPrepareCallback {
    /**
     * 准备成功回调
     *
     * @param prepare   ffmpeg是否准备成功
     * @param totalTime 音频总时长
     */
    void onPrepare(boolean prepare, int totalTime);
}
