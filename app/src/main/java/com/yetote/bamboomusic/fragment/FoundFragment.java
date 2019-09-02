package com.yetote.bamboomusic.fragment;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.yetote.bamboomusic.R;
import com.yetote.bamboomusic.VideoDetailsActivity;
import com.yetote.bamboomusic.adapter.FoundAdapter;
import com.yetote.bamboomusic.adapter.RecyclerViewItemClickListener;
import com.yetote.bamboomusic.media.MusicService;
import com.yetote.bamboomusic.media.OnFFmpegCallback;
import com.yetote.bamboomusic.model.FoundModel;

import java.util.ArrayList;

import static android.content.Context.BIND_AUTO_CREATE;
import static com.yetote.bamboomusic.media.MusicService.SERVICE_IN_FRAGMENT;
import static com.yetote.bamboomusic.media.MusicService.STATE_PAUSE;
import static com.yetote.bamboomusic.media.MusicService.STATE_PLAYING;
import static com.yetote.bamboomusic.media.MusicService.STATE_PREPARE;
import static com.yetote.bamboomusic.media.MusicService.STATE_STOP;
import static com.yetote.bamboomusic.media.MyPlayer.MEDIA_VIDEO;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.fragment
 * @class describe
 * @time 2019/8/6 13:38
 * @change
 * @chang time
 * @class describe
 */
public class FoundFragment extends Fragment {
    private RecyclerView recyclerView;
    private FoundAdapter adapter;
    private ArrayList<FoundModel> list;
    private static final String TAG = "FoundFragment";
    private MusicService.MusicBinder musicBinder;
    private SeekBar seekBar;
    private Surface surface;
    private int width, height;
    private static int playPos;
    private OnFFmpegCallback onFFmpegCallback;
    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {

            musicBinder = (MusicService.MusicBinder) service;
            callBack();
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Log.e(TAG, "onServiceDisconnected: 销毁service");
        }
    };

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_found, null);
        initView(v);
        onFFmpegCallback = new OnFFmpegCallback() {
            @Override
            public void onFFmpegPrepare(boolean prepare, int totalTime) {
                if (prepare) {
                    if (seekBar != null) {
                        seekBar.setMax(totalTime);
                    }
                    musicBinder.play(surface, width, height);
                }
            }

            @Override
            public void onFFmpegPlaying(int currentTime) {
                if (seekBar != null) {
                    seekBar.setProgress(currentTime);
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
        };
        Intent musicService = new Intent(getContext(), MusicService.class);
        getContext().bindService(musicService, serviceConnection, BIND_AUTO_CREATE);

        return v;
    }

    private void initView(View v) {
        recyclerView = v.findViewById(R.id.fragment_found_rv);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        list = new ArrayList<>();
//        list.add(new FoundModel(getContext().getExternalFilesDir(null).getPath() + "/1.mp4", 11, 11, "111"));
        list.add(new FoundModel("http://fs.mv.web.kugou.com/201909022018/dcf610a4df507edfd6943559990e5a11/G158/M06/0E/03/PocBAF0UMW6AS_dGAJZBOJlTixY276.mp4", 11, 11, "111"));
        list.add(new FoundModel("http://fs.mv.web.kugou.com/201909022012/47cf7e27370e6a682e1b58eb637998a0/G135/M01/10/01/xw0DAFtrkvOARUjvAyqky59_O0E064.mp4", 11, 11, "111"));
        list.add(new FoundModel("http://fs.mv.web.kugou.com/201909022020/2b5a6feef8318a07ccd226828743f236/G033/M08/1E/17/AZQEAFXdphCAbSX7AOF4GmV6uHs403.mp4", 11, 11, "111"));
//        list.add(new FoundModel(getContext().getExternalFilesDir(null).getPath() + "/test.mp4", 11, 11, "111"));
        adapter = new FoundAdapter(list, getContext());
        recyclerView.setAdapter(adapter);


        adapter.setRecyclerViewItemClickListener(new RecyclerViewItemClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent();
                if (musicBinder != null) {
                    if (musicBinder.getState() != STATE_STOP) {
                        i.putExtra("currentTime", seekBar.getProgress());
                    }
                    musicBinder.stop();
                    i.putExtra("path", list.get((Integer) v.getTag(R.id.music_found_tag_position)).getPath());
                    i.setClass(getContext(), VideoDetailsActivity.class);
                    startActivity(i);
                }
            }

            @Override
            public void onItemClick(View v, View parent, int pos) {
                switch (v.getId()) {
                    case R.id.rv_music_found_item_start:
                        if (musicBinder != null) {
                            if (musicBinder.getLocal() != SERVICE_IN_FRAGMENT) {
                                musicBinder.setServiceFFmpegCallBack(null);
                                musicBinder.setServiceFFmpegCallBack(onFFmpegCallback);
                            }
                            switch (musicBinder.getState()) {
                                case STATE_PLAYING:
                                    if (playPos == pos) {
                                        musicBinder.pause();
                                    } else {
                                        //切换视频资源
                                        musicBinder.stop();
                                        playPos = pos;
                                        seekBar = parent.findViewById(R.id.rv_music_found_item_seek);
                                        surface = (Surface) parent.getTag(R.id.music_found_tag_surface);
                                        width = (int) parent.getTag(R.id.music_found_tag_width);
                                        height = (int) parent.getTag(R.id.music_found_tag_height);
                                        musicBinder.prepare((String) parent.getTag(R.id.music_found_tag_path), MEDIA_VIDEO);
                                    }
                                    break;
                                case STATE_STOP:
                                    if (playPos != pos) {
                                        //切换视频资源
                                        if (musicBinder.getState() != STATE_STOP) {
                                            musicBinder.stop();
                                        }
                                        playPos = pos;
                                    }
                                    seekBar = parent.findViewById(R.id.rv_music_found_item_seek);
                                    surface = (Surface) parent.getTag(R.id.music_found_tag_surface);
                                    width = (int) parent.getTag(R.id.music_found_tag_width);
                                    height = (int) parent.getTag(R.id.music_found_tag_height);
                                    musicBinder.prepare((String) parent.getTag(R.id.music_found_tag_path), MEDIA_VIDEO);
                                    break;
                                case STATE_PAUSE:
                                    if (playPos == pos) {
                                        musicBinder.resume();
                                    } else {
                                        //切换视频资源
                                        musicBinder.stop();
                                        playPos = pos;
                                        seekBar = parent.findViewById(R.id.rv_music_found_item_seek);
                                        surface = (Surface) parent.getTag(R.id.music_found_tag_surface);
                                        width = (int) parent.getTag(R.id.music_found_tag_width);
                                        height = (int) parent.getTag(R.id.music_found_tag_height);
                                        musicBinder.prepare((String) parent.getTag(R.id.music_found_tag_path), MEDIA_VIDEO);
                                    }
                                    break;
                                case STATE_PREPARE:
                                    break;
                                default:
                                    Toast.makeText(getContext(), "未知错误", Toast.LENGTH_SHORT).show();
                                    break;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        });
    }


    private void callBack() {
        musicBinder.setServiceFFmpegCallBack(onFFmpegCallback);
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.e(TAG, "onPause: ");
        if (musicBinder != null) {
            musicBinder.stop();
        }
    }
}
