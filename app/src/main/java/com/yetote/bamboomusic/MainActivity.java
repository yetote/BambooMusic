package com.yetote.bamboomusic;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
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
import com.yetote.bamboomusic.myview.MusicProgressButton;
import com.yetote.bamboomusic.util.TextUtil;

import java.util.ArrayList;

import static android.widget.ListPopupWindow.MATCH_PARENT;
import static android.widget.ListPopupWindow.WRAP_CONTENT;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
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

    /**
     * 播放状态
     */
    enum PLAY_STATE {
        /**
         * 准备中
         */
        PREPARING,
        /**
         * 播放
         */
        PLAYING,
        /**
         * 暂停
         */
        PAUSING,
        /**
         * 停止
         */
        STOP
    }

    private PLAY_STATE state = PLAY_STATE.PREPARING;
    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.e(TAG, "onServiceConnected: service connect");
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
                musicBinder.resume();
            }
        });

        Intent musicService = new Intent(this, MusicService.class);
        bindService(musicService, serviceConnection, BIND_AUTO_CREATE);

        viewPager.setAdapter(viewPagerAdapter);
        tabLayout.setupWithViewPager(viewPager);


    }

    private void callBack() {
        musicBinder.setOnPrepareCallback((prepare, totalTime) -> {
            Toast.makeText(MainActivity.this, "打开" + totalTime, Toast.LENGTH_SHORT).show();
            musicDetailsPopTotalTime.setText(TextUtil.time2err(totalTime));
            musicDetailsPopProgress.setMax(totalTime);
            musicProgressButton.setTotalTime(totalTime);
            state = PLAY_STATE.PLAYING;
            musicBinder.play();
        });

        musicBinder.setPlayCallback(currentTime -> {
            musicDetailsPopProgress.setProgress(currentTime);
            if (musicProgressButton.getPlayState() != MusicProgressButton.STATE_PLAYING) {
                musicProgressButton.changeState(MusicProgressButton.STATE_PLAYING);
            }
            musicProgressButton.showPlayingAnimation(currentTime);
            musicDetailsPopCurrentTime.setText(TextUtil.time2err(currentTime));
        });
    }

    private void initView() {
        musicProgressButton = findViewById(R.id.main_musicProgress_btn);
        toolbar = findViewById(R.id.main_toolbar);
        myMusicQueueIv = findViewById(R.id.main_musicQueue_ImageView);
        path = getExternalFilesDir(null).getPath() + "/test.avi";
        viewPager = findViewById(R.id.viewPager);
        tabLayout = findViewById(R.id.tabLayout);
        fragments = new ArrayList<>();
        title = new ArrayList<>();
        fragments.add(new MusicLibFragment());
        fragments.add(new RecommendFragment());
        fragments.add(new FoundFragment());
        fragments.add(new MineFragment());
        title.add("音乐馆");
        title.add("推荐");
        title.add("发现");
        title.add("我的");
        viewPagerAdapter = new ViewPagerAdapter(getSupportFragmentManager(), fragments, title);
        musicIcon = findViewById(R.id.main_music_playing_icon);
        initPopupWindow();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.main_musicQueue_ImageView:
                musicQueuePopupWindow.showAsDropDown(v);
                break;
            case R.id.main_musicProgress_btn:
                if (musicBinder != null) {
                    switch (state) {
                        case PREPARING:
                            musicBinder.prepare(path);
                            musicProgressButton.changeState(MusicProgressButton.STATE_PROGRESS);
                            break;
                        case PLAYING:
                            musicBinder.pause();
                            state = PLAY_STATE.PAUSING;
                            musicProgressButton.changeState(MusicProgressButton.STATE_STOP);
                            break;
                        case PAUSING:
                            musicBinder.resume();
                            state = PLAY_STATE.PLAYING;
                            break;
                        case STOP:
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
        musicDetailsPopCurrentTime = musicDetailsView.findViewById(R.id.music_details_pop_currentTime);
        musicDetailsPopTotalTime = musicDetailsView.findViewById(R.id.music_details_pop_totalTime);
        musicDetailsPopProgress = musicDetailsView.findViewById(R.id.music_details_pop_progress);
        musicDetailsPopupWindow.setClippingEnabled(false);
    }
}
