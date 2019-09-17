package com.yetote.bamboomusic;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Environment;
import android.os.IBinder;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatSeekBar;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.RecyclerView;
import androidx.viewpager.widget.ViewPager;

import com.google.android.material.tabs.TabLayout;
import com.yetote.bamboomusic.adapter.ViewPagerAdapter;
import com.yetote.bamboomusic.fragment.FoundFragment;
import com.yetote.bamboomusic.fragment.MineFragment;
import com.yetote.bamboomusic.fragment.MusicLibFragment;
import com.yetote.bamboomusic.fragment.RecommendFragment;
import com.yetote.bamboomusic.media.MusicService;
import com.yetote.bamboomusic.media.OnFFmpegCallback;
import com.yetote.bamboomusic.myview.MusicProgressButton;
import com.yetote.bamboomusic.util.TextUtil;

import java.lang.reflect.Field;
import java.util.ArrayList;

import io.reactivex.Observable;
import io.reactivex.android.schedulers.AndroidSchedulers;

import static android.widget.ListPopupWindow.MATCH_PARENT;
import static android.widget.ListPopupWindow.WRAP_CONTENT;
import static com.yetote.bamboomusic.media.MusicService.SERVICE_IN_MAIN_ACTIVITY;
import static com.yetote.bamboomusic.media.MusicService.STATE_PAUSE;
import static com.yetote.bamboomusic.media.MusicService.STATE_PLAYING;
import static com.yetote.bamboomusic.media.MusicService.STATE_PREPARE;
import static com.yetote.bamboomusic.media.MusicService.STATE_STOP;
import static com.yetote.bamboomusic.media.MyPlayer.MEDIA_AUDIO;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, OnFFmpegCallback {
    private static final String TAG = "MainActivity";
    private MusicProgressButton musicProgressButton;
    private ImageView musicIcon;
    private Toolbar toolbar;
    private ImageView myMusicQueueIv;
    private View musicQueueView;
    private ImageView musicQueuePopChangePlayMode, musicQueuePopDownload, musicQueuePopAdd, musicQueuePopDelete;
    private ImageView musicDetailsPopPlayMode, musicDetailsPopOn, musicDetailsPopUnder, musicDetailsPopPlayController, musicDetailsPopMusicQueue, musicDetailsPopLike, musicDetailsPopDownload, musicDetailsPopShare, musicDetailsPopDiscuss;
    private Toolbar musicDetailsPopToolbar;
    private TextView musicDetailsPopCurrentTime, musicDetailsPopTotalTime;
    private AppCompatSeekBar musicDetailsPopProgress;
    private TextView musicQueuePopPlayMode, musicQueuePopBack;
    private RecyclerView musicQueuePopMusicQueue;
    private PopupWindow musicQueuePopupWindow, musicDetailsPopupWindow;
    private ViewPager viewPager;
    private TabLayout tabLayout;
    private ViewPagerAdapter viewPagerAdapter;
    private ArrayList<Fragment> fragments;
    private ArrayList<String> title;
    private MusicService.MusicBinder musicBinder;
    private View musicDetailsView;
    private boolean isPlaying;
    private boolean isPause;
    private String path;
    private ArrayList<String> musicList;
    private static int playingPos = 0;

    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.e(TAG, "onServiceConnected: service connect");
            if (musicBinder != null) {
                musicBinder.stop();
            }
            musicBinder = (MusicService.MusicBinder) service;
            callBack();
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initView();

        toolbar.inflateMenu(R.menu.main_toolbar_menu);
        toolbar.setTitle("音乐馆");

        myMusicQueueIv.setOnClickListener(this);
        musicProgressButton.setOnClickListener(this);
        musicIcon.setOnClickListener(this);

        musicDetailsPopProgress.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
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

            }
        });

        Intent musicService = new Intent(this, MusicService.class);
        bindService(musicService, serviceConnection, BIND_AUTO_CREATE);

        viewPager.setAdapter(viewPagerAdapter);
        tabLayout.setupWithViewPager(viewPager);
    }

    private void callBack() {
        musicBinder.setServiceFFmpegCallBack(this);
    }

    private void initView() {
        path = getExternalFilesDir(null).getPath() + "/test.avi";
        Log.e(TAG, "initView: " + path);
        musicIcon = findViewById(R.id.main_music_playing_icon);

        musicProgressButton = findViewById(R.id.main_musicProgress_btn);
        toolbar = findViewById(R.id.main_toolbar);
        myMusicQueueIv = findViewById(R.id.main_musicQueue_ImageView);
        viewPager = findViewById(R.id.viewPager);
        tabLayout = findViewById(R.id.tabLayout);
        initPopupWindow();

        fragments = new ArrayList<>();
        title = new ArrayList<>();
        musicList = new ArrayList<>();

        fragments.add(new MusicLibFragment());
        fragments.add(new RecommendFragment());
        fragments.add(new FoundFragment());
        fragments.add(new MineFragment());
        title.add("音乐馆");
        title.add("推荐");
        title.add("发现");
        title.add("我的");

//        musicList.add("http://fs.open.kugou.com/96fd6b9d8795dd8265dc0d4260bfe129/5d6e65c5/G095/M00/18/00/P5QEAFussHyAMPWEADaRkzG3ZHk167.mp3");
//        musicList.add("http://fs.open.kugou.com/6f91469d548bad01a56eb4254bb69efe/5d6e6861/G153/M04/11/17/2Q0DAF0D61uAHxNTADuPhCASo0k439.mp3");
//        musicList.add("http://fs.open.kugou.com/f3bd8f6aeb4efab069604da80d03b4af/5d7f8b6f/G079/M02/1A/04/jw0DAFhHEBmAMsPYAChSJvSXqn4945.mp3");
//        musicList.add("http://fs.open.kugou.com/a77f152e7922c90d8a9f44422cfe2273/5d7f8b46/G006/M06/1E/17/Rg0DAFS4zk-ASqyoABLcGPv2a0M093.mp3");
//        musicList.add("http://fs.open.kugou.com/ce42151c7db957f311e60484984529ac/5d7f83be/G153/M04/13/14/OYcBAFz3fF6AbF0fADS_2OPt0ag626.mp3");
//        musicList.add(getExternalFilesDir(null).getPath() + "/test.aac");
        musicList.add(getExternalFilesDir(Environment.DIRECTORY_MUSIC).getPath() + "/1.mp3");
        musicList.add(getExternalFilesDir(Environment.DIRECTORY_MUSIC).getPath() + "/2.mp3");
        musicList.add(getExternalFilesDir(Environment.DIRECTORY_MUSIC).getPath() + "/3.mp3");
        musicList.add(getExternalFilesDir(Environment.DIRECTORY_MUSIC).getPath() + "/4.mp3");
        musicList.add(getExternalFilesDir(Environment.DIRECTORY_MUSIC).getPath() + "/5.mp3");
        musicList.add(getExternalFilesDir(Environment.DIRECTORY_MUSIC).getPath() + "/6.mp3");

        viewPagerAdapter = new ViewPagerAdapter(getSupportFragmentManager(), fragments, title);

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.main_musicQueue_ImageView:
                musicQueuePopupWindow.showAsDropDown(v);
                break;
            case R.id.main_musicProgress_btn:
                if (musicBinder != null) {

                    if (musicBinder.getLocal() != SERVICE_IN_MAIN_ACTIVITY) {
                        musicBinder.setServiceFFmpegCallBack(null);
                        musicBinder.setServiceFFmpegCallBack(this);
                        Log.e(TAG, "onClick: 重设service接口");
                    }
                    Log.e(TAG, "onClick: state为" + musicBinder.getState());
                    switch (musicBinder.getState()) {
                        case STATE_STOP:
                            musicProgressButton.changeState(MusicProgressButton.STATE_PROGRESS);
                            musicBinder.prepare(musicList.get(playingPos), MEDIA_AUDIO);
                            break;
                        case STATE_PLAYING:
                            musicBinder.pause();
                            musicDetailsPopPlayController.setBackground(getDrawable(R.drawable.music_state_play));
                            break;
                        case STATE_PAUSE:
                            musicBinder.resume();
                            break;
                        case STATE_PREPARE:
                            break;
                        default:
                            break;
                    }
                }
                break;
            case R.id.main_music_playing_icon:
                Log.e(TAG, "onClick: ");
                musicDetailsPopupWindow.showAtLocation(v, Gravity.CENTER, 0, 0);
                break;
            default:
                Log.e(TAG, "onClick: 点击id" + v.getId());
                break;
        }
    }

    private void initPopupWindow() {
        musicQueueView = LayoutInflater.from(this).inflate(R.layout.popupwindow_main_music_queue, null);
        musicQueuePopChangePlayMode = musicQueueView.findViewById(R.id.main_pop_change_playMode);
        musicQueuePopDownload = musicQueueView.findViewById(R.id.main_pop_download);
        musicQueuePopAdd = musicQueueView.findViewById(R.id.main_pop_add);
        musicQueuePopDelete = musicQueueView.findViewById(R.id.main_pop_delete);
        musicQueuePopPlayMode = musicQueueView.findViewById(R.id.main_pop_playMode);
        musicQueuePopBack = musicQueueView.findViewById(R.id.main_pop_back);
        musicQueuePopMusicQueue = musicQueueView.findViewById(R.id.main_pop_musicQueue_rv);
        musicQueuePopupWindow = new PopupWindow(musicQueueView, MATCH_PARENT, WRAP_CONTENT, false);

        int stateBarSize = 0;
        int resourceId = getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            stateBarSize = getResources().getDimensionPixelSize(resourceId);
        }
        musicDetailsView = LayoutInflater.from(this).inflate(R.layout.popupwindow_music_details, null);
        musicDetailsPopupWindow = new PopupWindow(musicDetailsView, MATCH_PARENT, MATCH_PARENT, true);
        musicDetailsPopPlayMode = musicDetailsView.findViewById(R.id.music_details_pop_playMode);
        musicDetailsPopOn = musicDetailsView.findViewById(R.id.music_details_pop_on);
        musicDetailsPopUnder = musicDetailsView.findViewById(R.id.music_details_pop_under);
        musicDetailsPopPlayController = musicDetailsView.findViewById(R.id.music_details_pop_playController);
        musicDetailsPopMusicQueue = musicDetailsView.findViewById(R.id.music_details_pop_musicQueue);
        musicDetailsPopLike = musicDetailsView.findViewById(R.id.music_details_pop_like);
        musicDetailsPopDownload = musicDetailsView.findViewById(R.id.music_details_pop_download);
        musicDetailsPopShare = musicDetailsView.findViewById(R.id.music_details_pop_share);
        musicDetailsPopDiscuss = musicDetailsView.findViewById(R.id.music_details_pop_discuss);
        musicDetailsPopToolbar = musicDetailsView.findViewById(R.id.music_details_pop_toolbar);
        musicDetailsPopToolbar.setTitleMarginTop(stateBarSize);
        musicDetailsPopCurrentTime = musicDetailsView.findViewById(R.id.music_details_pop_currentTime);
        musicDetailsPopTotalTime = musicDetailsView.findViewById(R.id.music_details_pop_totalTime);
        musicDetailsPopProgress = musicDetailsView.findViewById(R.id.music_details_pop_progress);
        musicDetailsPopupWindow.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE);

        try {
            Field mLayoutInScreen = PopupWindow.class.getDeclaredField("mLayoutInScreen");
            mLayoutInScreen.setAccessible(true);
            mLayoutInScreen.set(musicDetailsPopupWindow, true);
        } catch (NoSuchFieldException | IllegalAccessException e) {
            e.printStackTrace();
        }
        musicDetailsPopupWindow.setWidth(WindowManager.LayoutParams.MATCH_PARENT);
        musicDetailsPopupWindow.setHeight(WindowManager.LayoutParams.MATCH_PARENT);

        musicDetailsPopUnder.setOnClickListener(v -> {
            if (musicBinder != null) {
                if (musicBinder.getState() != STATE_STOP) {
                    musicBinder.stop();
                }
                playingPos++;
                if (playingPos >= musicList.size()) {
                    playingPos = 0;
                }
                musicProgressButton.changeState(MusicProgressButton.STATE_PROGRESS);
                musicBinder.prepare(musicList.get(playingPos), MEDIA_AUDIO);
            }
        });

        musicDetailsPopOn.setOnClickListener(v -> {
            if (musicBinder != null) {
                if (musicBinder.getState() != STATE_STOP) {
                    musicBinder.stop();
                }
                playingPos--;
                if (playingPos < 0) {
                    playingPos = musicList.size() - 1;
                }
                musicProgressButton.changeState(MusicProgressButton.STATE_PROGRESS);
                musicBinder.prepare(musicList.get(playingPos), MEDIA_AUDIO);
            }
        });

        musicDetailsPopPlayController.setOnClickListener(v -> {
            if (musicBinder != null) {
                if (musicBinder.getState() == STATE_PAUSE) {
                    musicBinder.resume();
                    musicDetailsPopPlayController.setBackground(getDrawable(R.drawable.music_state_pause));
                } else if (musicBinder.getState() == STATE_PLAYING) {
                    musicBinder.pause();
                    musicDetailsPopPlayController.setBackground(getDrawable(R.drawable.music_state_play));
                } else if (musicBinder.getState() == STATE_STOP) {
                    musicProgressButton.changeState(MusicProgressButton.STATE_PROGRESS);
                    musicBinder.prepare(musicList.get(playingPos), MEDIA_AUDIO);
                    musicDetailsPopPlayController.setBackground(getDrawable(R.drawable.music_state_pause));
                }
            }
        });

    }

    @Override
    public void onFFmpegPrepare(boolean prepare, int totalTime) {
        if (prepare) {
            musicDetailsPopTotalTime.setText(TextUtil.time2Str(totalTime));
            musicDetailsPopProgress.setMax(totalTime);
            musicProgressButton.setTotalTime(totalTime);
            musicBinder.play();
            Log.e(TAG, "onFFmpegPrepare: 准备播放");
        } else {
            musicProgressButton.changeState(MusicProgressButton.STATE_STOP);
            musicBinder.stop();
            Log.e(TAG, "onFFmpegPrepare: 打开文件失败");
            Observable.create(emitter -> Toast.makeText(this, "打开文件失败", Toast.LENGTH_SHORT).show())
                    .subscribeOn(AndroidSchedulers.mainThread())
                    .subscribe(o -> {
                    });

        }
    }

    @Override
    public void onFFmpegPlaying(int currentTime) {
        musicDetailsPopProgress.setProgress(currentTime);
        if (musicProgressButton.getPlayState() != MusicProgressButton.STATE_PLAYING) {
            musicProgressButton.changeState(MusicProgressButton.STATE_PLAYING);
        }
        musicProgressButton.showPlayingAnimation(currentTime);
        musicDetailsPopCurrentTime.setText(TextUtil.time2Str(currentTime));
        musicDetailsPopPlayController.setBackground(getDrawable(R.drawable.music_state_pause));
        for (String s : musicList) {

        }
    }

    @Override
    public void onFFmpegPause() {
        musicProgressButton.changeState(MusicProgressButton.STATE_STOP);
    }

    @Override
    public void onFFmpegResume() {

    }

    @Override
    public void onFFmpegSeek() {
        musicBinder.resume();
    }

    @Override
    public void onFFmpegStop() {
        musicProgressButton.changeState(MusicProgressButton.STATE_STOP);
    }
}
