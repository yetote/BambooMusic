package com.yetote.bamboomusic.media;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.media
 * @class describe
 * @time 2019/8/29 15:20
 * @change
 * @chang time
 * @class describe
 */
public interface OnFFmpegCallback {
    /**
     * 准备成功回调
     *
     * @param prepare   ffmpeg是否准备成功
     * @param totalTime 音频总时长
     */
    void onFFmpegPrepare(boolean prepare, int totalTime);

    /**
     * 播放回调
     *
     * @param currentTime 当前播放时间
     */
    void onFFmpegPlaying(int currentTime);

    /**
     * 暂停回调
     */
    void onFFmpegPause();

    /**
     * 恢复播放回调
     */
    void onFFmpegResume();

    /**
     * 销毁回调
     */
    void onFFmpegStop();
}
