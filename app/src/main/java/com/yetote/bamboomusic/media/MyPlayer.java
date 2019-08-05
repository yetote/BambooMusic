package com.yetote.bamboomusic.media;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.com.yetote.bamboomusic.media
 * @class 播放器
 * @time 2019/8/5 16:20
 * @change
 * @chang time
 * @class describe
 */
public class MyPlayer {
    static {
        System.loadLibrary("native-lib");
    }

    private OnPrepareCallback prepareCallback;

    public native void prepare(String path);

    public native void play();

    public native void pause();

    public native void stop();

    public void setPrepareCallback(OnPrepareCallback prepareCallback) {
        this.prepareCallback = prepareCallback;
    }

    public void callPrepare(boolean isPrepare) {
        if (prepareCallback != null) {
            prepareCallback.onPrepare(isPrepare);
        }
    }

}
