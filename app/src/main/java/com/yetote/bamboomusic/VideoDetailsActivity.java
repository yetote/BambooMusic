package com.yetote.bamboomusic;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.RecyclerView;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.yetote.bamboomusic.media.MusicService;
import com.yetote.bamboomusic.media.OnFFmpegCallback;

import static com.yetote.bamboomusic.media.MusicService.SERVICE_IN_VIDEO_DETAILS_ACTIVITY;
import static com.yetote.bamboomusic.media.MyPlayer.MEDIA_VIDEO;

public class VideoDetailsActivity extends AppCompatActivity implements OnFFmpegCallback {
    private SurfaceView surfaceView;
    private RecyclerView recyclerView;
    private MusicService.MusicBinder musicBinder;
    private Surface surface;
    private int width, height;
    private static final String TAG = "VideoDetailsActivity";
    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {

            musicBinder = (MusicService.MusicBinder) service;
            callBack();
            musicBinder.prepare(path, MEDIA_VIDEO);
            Log.e(TAG, "onServiceConnected: connect");
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Log.d(TAG, "onServiceDisconnected: 销毁service");
        }
    };
    private int progress;
    private String path;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_details);

        Intent i = getIntent();
        progress = i.getIntExtra("currentTime", 0);
        path = i.getStringExtra("path");
        initView();
        Intent musicService = new Intent(this, MusicService.class);
        bindService(musicService, serviceConnection, BIND_AUTO_CREATE);


    }

    private void initView() {
        surfaceView = findViewById(R.id.video_details_sv);
        recyclerView = findViewById(R.id.video_details_rv);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                surface = holder.getSurface();

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
                width = w;
                height = h;

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

    private void callBack() {
        if (musicBinder.getLocal() != SERVICE_IN_VIDEO_DETAILS_ACTIVITY) {
            musicBinder.setServiceFFmpegCallBack(null);
        }
        musicBinder.setServiceFFmpegCallBack(this);
    }

    @Override
    public void onFFmpegPrepare(boolean prepare, int totalTime) {
        if (prepare) {
            if (surface != null) {
                Log.e(TAG, "onFFmpegPrepare: w" + width + "h" + height);
                musicBinder.play(surface, width, height);

            } else {
                Log.e(TAG, "onFFmpegPrepare: surface为空");
            }
        } else {
            Log.e(TAG, "onFFmpegPrepare: 准备失败");
        }
    }

    @Override
    public void onFFmpegPlaying(int currentTime) {
        if (progress != 0) {
            musicBinder.pause();
            musicBinder.seek(progress);
            musicBinder.resume();
            progress = 0;
        }
    }

    @Override
    public void onFFmpegPause() {

    }

    @Override
    public void onFFmpegResume() {

    }

    @Override
    public void onFFmpegStop() {

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (musicBinder != null) {
            musicBinder.stop();
            unbindService(serviceConnection);
        }

    }
}
