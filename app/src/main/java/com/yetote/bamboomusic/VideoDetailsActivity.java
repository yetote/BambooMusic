package com.yetote.bamboomusic;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatSeekBar;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.util.TimeUtils;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import com.yetote.bamboomusic.media.MusicService;
import com.yetote.bamboomusic.media.OnFFmpegCallback;
import com.yetote.bamboomusic.util.TextUtil;

import static android.content.pm.ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
import static android.view.View.INVISIBLE;
import static android.view.View.SYSTEM_UI_FLAG_FULLSCREEN;
import static android.view.View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
import static android.view.View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
import static android.view.WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS;
import static com.yetote.bamboomusic.media.MusicService.SERVICE_IN_VIDEO_DETAILS_ACTIVITY;
import static com.yetote.bamboomusic.media.MyPlayer.MEDIA_VIDEO;

public class VideoDetailsActivity extends AppCompatActivity implements OnFFmpegCallback {
    private SurfaceView surfaceView;
    private RecyclerView recyclerView;
    private MusicService.MusicBinder musicBinder;
    private Surface surface;
    private int width, height;
    private AppCompatSeekBar seekBar;
    private TextView currentTv, totalTv;
    private ImageView full;
    private boolean isFullScreen = false;
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
//        getWindow().addFlags(FLAG_TRANSLUCENT_STATUS);
        setContentView(R.layout.activity_video_details);

        Intent i = getIntent();
        progress = i.getIntExtra("currentTime", 0);
        path = i.getStringExtra("path");
        initView();
        Intent musicService = new Intent(this, MusicService.class);
        bindService(musicService, serviceConnection, BIND_AUTO_CREATE);

        onClick();

    }

    private void onClick() {
        full.setOnClickListener(v -> {
            setRequestedOrientation(SCREEN_ORIENTATION_LANDSCAPE);
            getWindow().getDecorView().setSystemUiVisibility(SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | SYSTEM_UI_FLAG_FULLSCREEN|SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

            ConstraintLayout.LayoutParams lp = new ConstraintLayout.LayoutParams(WindowManager.LayoutParams.MATCH_PARENT, WindowManager.LayoutParams.MATCH_PARENT);
            surfaceView.setLayoutParams(lp);
            isFullScreen = true;
            seekBar.setVisibility(INVISIBLE);
        });
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                musicBinder.pause();
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                musicBinder.seek(seekBar.getProgress());
                musicBinder.resume();
            }
        });
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
                if (isFullScreen) {
                    Log.e(TAG, "surfaceChanged: width" + width + "height" + height);
                    musicBinder.fullScreen(width, height);
                    isFullScreen = false;
                }
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });

        currentTv = findViewById(R.id.video_details_currentTime);
        totalTv = findViewById(R.id.video_details_totalTime);
        full = findViewById(R.id.video_details_fullScreen);
        seekBar = findViewById(R.id.video_details_seekBar);
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
                seekBar.setMax(totalTime);
                totalTv.setText(TextUtil.time2Str(totalTime));
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
        currentTv.setText(TextUtil.time2Str(currentTime));
        seekBar.setProgress(currentTime);
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
            musicBinder = null;
            unbindService(serviceConnection);
        }
    }
}
