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

    public static final int MEDIA_AUDIO = 0;
    public static final int MEDIA_VIDEO = 1;
    private static final String TAG = "MyPlayer";
    private Context context;

    public MyPlayer(Context context) {
        this.context = context;
    }

    private OnFFmpegCallback ffmpegCallback;

    public native void prepare(String path, int type);


    public native void play();

    public native void play(Surface surface, int w, int h, String vertexCodec, String fragCode);

    public native void pause();

    public native void stop();

    public native void resume();

    public native void seek(int progress);

    public void setffmpegCallback(OnFFmpegCallback ffmpegCallback) {
        this.ffmpegCallback = ffmpegCallback;
    }

    public void callPrepare(boolean isPrepare, int totalTime) {
        if (ffmpegCallback != null) {
            ffmpegCallback.onFFmpegPrepare(isPrepare, totalTime);
        }
    }

    public void callPlay(int currentTime) {
        if (ffmpegCallback != null) {
            ffmpegCallback.onFFmpegPlaying(currentTime);
        }
    }

    public void callPause() {
        if (ffmpegCallback != null) {
            ffmpegCallback.onFFmpegPause();
        }
    }

    public void callResume() {
        if (ffmpegCallback != null) {
            ffmpegCallback.onFFmpegResume();
        }
    }

    public void callStop() {
        if (ffmpegCallback != null) {
            ffmpegCallback.onFFmpegStop();
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
