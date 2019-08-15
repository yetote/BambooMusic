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
    public static final int STATE_PREPARE = 1;
    public static final int STATE_PLAYING = 2;
    public static final int STATE_PAUSE = 3;
    public static final int STATE_STOP = 4;
    private int state = STATE_STOP;

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
        player.setPrepareCallback((prepare, totalTime) -> {
            musicBinder.callPrepare(prepare, totalTime);
            if (prepare) {
                player.play();
                state = STATE_PLAYING;
            }
        });

        player.setPlayCallback(currentTime -> {
            musicBinder.callPlay(currentTime);
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

        private OnPrepareCallback onPrepareCallback;

        public void setOnPrepareCallback(OnPrepareCallback onPrepareCallback) {
            this.onPrepareCallback = onPrepareCallback;
        }

        private OnPlayCallback onPlayCallback;

        public void setPlayCallback(OnPlayCallback playCallback) {
            this.onPlayCallback = playCallback;
        }

        protected void callPrepare(boolean success, int totalTime) {
            onPrepareCallback.onPrepare(success, totalTime);
        }

        protected void callPlay(int currentTime) {
            onPlayCallback.onPlaying(currentTime);
        }

        public void play(String path) {
            if (player != null) {
                player.prepare(path);
                state = STATE_PREPARE;
            }
        }

        public void play(String path, Surface surface, int w, int h) {
            if (player != null) {
                player.prepare(path, surface, w, h, TextRecourseReader.readTextFileFromResource(getApplicationContext(), R.raw.yuv_vertex_shader), TextRecourseReader.readTextFileFromResource(getApplicationContext(), R.raw.yuv_frag_shader));
                state = STATE_PREPARE;
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
                state = STATE_PLAYING;
            }
        }
    }

}
