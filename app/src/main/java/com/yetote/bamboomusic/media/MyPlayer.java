package com.yetote.bamboomusic.media;

import android.content.Context;
import android.util.Log;
import android.view.Surface;

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

    private static final String TAG = "MyPlayer";
    private Context context;

    public MyPlayer(Context context) {
        this.context = context;
    }

    private OnPrepareCallback prepareCallback;
    private OnPlayCallback playCallback;

    public native void prepare(String path);

    public native void prepare(String path, Surface surface, int w, int h);

    public native void play();

    public native void pause();

    public native void stop();

    public native void resume();

    public native void seek(int progress);

    public void setPrepareCallback(OnPrepareCallback prepareCallback) {
        this.prepareCallback = prepareCallback;
    }

    public void setPlayCallback(OnPlayCallback playCallback) {
        this.playCallback = playCallback;
    }

    public void callPrepare(boolean isPrepare, int totalTime) {
        if (prepareCallback != null) {
            prepareCallback.onPrepare(isPrepare, totalTime);
        }
    }

    public void callPlay(int currentTime) {
        if (prepareCallback != null) {
            playCallback.onPlaying(currentTime);
        }
    }

    public boolean callHardwareSupport(String name) {
        return MediaUtil.hardwareSupport(name);
    }

    public void callHardwareCodec(String path) {
        Log.e(TAG, "callHardwareCodec: " + path);
        MediaUtil mediaUtil = new MediaUtil(context);
        mediaUtil.codec(path);
    }
}
