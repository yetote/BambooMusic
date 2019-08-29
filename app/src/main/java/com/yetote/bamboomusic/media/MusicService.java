package com.yetote.bamboomusic.media;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.view.Surface;

import androidx.annotation.Nullable;

import com.yetote.bamboomusic.R;
import com.yetote.bamboomusic.util.TextRecourseReader;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic
 * @class describe
 * @time 2019/8/6 13:22
 * @change
 * @chang time
 * @class describe
 */
public class MusicService extends Service {
    private static final String TAG = "MusicService";
    private MusicBinder musicBinder = new MusicBinder();
    private MyPlayer player;
    public static final int STATE_PREPARE = 0x0001;
    public static final int STATE_PLAYING = 0x0002;
    public static final int STATE_PAUSE = 0x0003;
    public static final int STATE_STOP = 0x0004;
    private int state = STATE_STOP;
    public static final int TYPE_AUDIO = 0x0005;
    public static final int TYPE_VIDEO = 0x0006;
    private Surface surface;
    private int w, h;
    private int type;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        Log.e(TAG, "onBind: bind");
        return musicBinder;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.e(TAG, "onCreate: service create");
        player = new MyPlayer(this);
        player.setffmpegCallback(new OnFFmpegCallback() {
            @Override
            public void onPrepare(boolean prepare, int totalTime) {
                musicBinder.callPrepare(prepare, totalTime);
            }

            @Override
            public void onPlaying(int currentTime) {
                musicBinder.callPlay(currentTime);
            }

            @Override
            public void onPause() {
                musicBinder.callPause();
            }

            @Override
            public void onResume() {
                musicBinder.callResume();
            }

            @Override
            public void onStop() {
                musicBinder.callStop();
            }


        });
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.e(TAG, "onCreate: service onStartCommand");
        return super.onStartCommand(intent, flags, startId);

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.e(TAG, "onDestroy: service destroy");
    }

    public class MusicBinder extends Binder {

        public int getState() {
            return state;
        }

        private OnFFmpegCallback serviceFFmpegCallBack;


        public void setServiceFFmpegCallBack(OnFFmpegCallback serviceFFmpegCallBack) {
            this.serviceFFmpegCallBack = serviceFFmpegCallBack;
        }


        void callPrepare(boolean success, int totalTime) {
            serviceFFmpegCallBack.onPrepare(success, totalTime);
        }

        void callPlay(int currentTime) {
            serviceFFmpegCallBack.onPlaying(currentTime);
        }

        private void callPause() {
            serviceFFmpegCallBack.onPause();
        }

        void callResume() {
            serviceFFmpegCallBack.onResume();
        }

        void callStop() {
            serviceFFmpegCallBack.onStop();
        }

        public void prepare(String path) {
            if (player != null) {
                player.prepare(path);
                state = STATE_PREPARE;
            }
        }

        public void play() {
            if (player != null) {
                player.play();
                state = STATE_PLAYING;
                type = TYPE_AUDIO;
            }
        }

        public void play(Surface surface, int w, int h) {
            if (player != null) {
                player.play(surface, w, h,
                        TextRecourseReader.readTextFileFromResource(getApplicationContext(), R.raw.yuv_vertex_shader)
                        , TextRecourseReader.readTextFileFromResource(getApplicationContext(), R.raw.yuv_frag_shader)
                );
                state = STATE_PREPARE;
                type = TYPE_VIDEO;
            }
        }

        public void resume() {
            if (player != null) {
                player.resume();
                state = STATE_PLAYING;
            }
        }

        public void pause() {
            if (player != null) {
                player.pause();
                state = STATE_PAUSE;
            }
        }

        public void seek(int progress) {
            if (player != null) {
                player.seek(progress);
            }
        }

        public void stop() {
            if (player != null) {
                player.stop();
                state = STATE_STOP;
            }
        }
    }

}
