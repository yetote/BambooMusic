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
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
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
import com.yetote.bamboomusic.media.OnPrepareCallback;
import com.yetote.bamboomusic.myview.MusicProgressButton;

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
    private ImageView popChangePlayMode, popDownload, popAdd, popDelete;
    private TextView popPlayMode, popBack;
    private RecyclerView popMusicQueue;
    private PopupWindow musicQueuePopupWindow, musicDetailsPopupWindow;
    private ViewPager viewPager;
    private TabLayout tabLayout;
    private ViewPagerAdapter viewPagerAdapter;
    private ArrayList<Fragment> fragments;
    private ArrayList<String> title;
    private MusicService.MusicBinder musicBinder;
    private View musicDetailsView;
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
        initPopupWindow();

        toolbar.inflateMenu(R.menu.main_toolbar_menu);
        toolbar.setTitle("音乐馆");

        myMusicQueueIv.setOnClickListener(this);
        musicProgressButton.setOnClickListener(this);
        musicIcon.setOnClickListener(this);

        Intent musicService = new Intent(this, MusicService.class);
        bindService(musicService, serviceConnection, BIND_AUTO_CREATE);

        viewPager.setAdapter(viewPagerAdapter);
        tabLayout.setupWithViewPager(viewPager);


    }

    private void callBack() {
        musicBinder.setOnPrepareCallback(new OnPrepareCallback() {
            @Override
            public void onPrepare(boolean prepare) {
                Toast.makeText(MainActivity.this, "打开" + prepare, Toast.LENGTH_SHORT).show();
            }
        });
    }

    private void initView() {
        musicProgressButton = findViewById(R.id.main_musicProgress_btn);
        toolbar = findViewById(R.id.main_toolbar);
        myMusicQueueIv = findViewById(R.id.main_musicQueue_ImageView);

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

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.main_musicQueue_ImageView:
                musicQueuePopupWindow.showAsDropDown(v);
                break;
            case R.id.main_musicProgress_btn:
                if (musicBinder != null) {
                    musicBinder.play("http://wsaudio.bssdlbig.kugou.com/1908082111/jYgNlG5nSmsauxMyqpdYaA/1565356291/bss/extname/wsaudio/bccd9d4f224f7fc0978efa29213370ff.mp3",this.getExternalFilesDir(null).getPath()+"/test.pcm");
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
        popChangePlayMode = musicQueueView.findViewById(R.id.main_pop_change_playMode);
        popDownload = musicQueueView.findViewById(R.id.main_pop_download);
        popAdd = musicQueueView.findViewById(R.id.main_pop_add);
        popDelete = musicQueueView.findViewById(R.id.main_pop_delete);
        popPlayMode = musicQueueView.findViewById(R.id.main_pop_playMode);
        popBack = musicQueueView.findViewById(R.id.main_pop_back);
        popMusicQueue = musicQueueView.findViewById(R.id.main_pop_musicQueue_rv);
        musicQueuePopupWindow = new PopupWindow(musicQueueView, MATCH_PARENT, WRAP_CONTENT, false);

        musicDetailsView = LayoutInflater.from(this).inflate(R.layout.popupwindow_music_details, null);
        musicDetailsPopupWindow = new PopupWindow(musicDetailsView, MATCH_PARENT, MATCH_PARENT, true);
    }


}
