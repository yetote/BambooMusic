package com.yetote.bamboomusic.media;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;

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
        player = new MyPlayer();
        player.setPrepareCallback(prepare -> {
            Log.e(TAG, "onCreate: " + prepare);
            musicBinder.callPrepare(prepare);
            if (prepare) {
                player.play();
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
        private OnPrepareCallback onPrepareCallback;

        public void setOnPrepareCallback(OnPrepareCallback onPrepareCallback) {
            this.onPrepareCallback = onPrepareCallback;
        }

        protected void callPrepare(boolean success) {
            onPrepareCallback.onPrepare(success);
        }

        public void play(String path,String wpath) {
            if (player != null) {
                player.prepare(path,wpath);
            }
        }

    }
}
