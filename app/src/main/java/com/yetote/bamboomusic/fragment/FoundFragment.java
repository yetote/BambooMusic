package com.yetote.bamboomusic.fragment;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.yetote.bamboomusic.MainActivity;
import com.yetote.bamboomusic.R;
import com.yetote.bamboomusic.adapter.FoundAdapter;
import com.yetote.bamboomusic.adapter.RecyclerViewItemClickListener;
import com.yetote.bamboomusic.media.MusicService;
import com.yetote.bamboomusic.model.FoundModel;
import com.yetote.bamboomusic.myview.MusicProgressButton;
import com.yetote.bamboomusic.util.TextUtil;

import java.util.ArrayList;

import static android.content.Context.BIND_AUTO_CREATE;
import static com.yetote.bamboomusic.media.MusicService.STATE_STOP;

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
    private SurfaceView surfaceView;
    int width, height;
    private SurfaceHolder surfaceHolder;
    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {

            Log.e(TAG, "onServiceConnected: service connect");
            musicBinder = (MusicService.MusicBinder) service;
            musicBinder.setOnPrepareCallback((prepare, totalTime) -> {
            });

            musicBinder.setPlayCallback(currentTime -> {

            });
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_found, null);
        Intent musicService = new Intent(getActivity(), MusicService.class);
        getContext().bindService(musicService, serviceConnection, BIND_AUTO_CREATE);
        initView(v);

        adapter.setItemClickListener(new RecyclerViewItemClickListener() {
            @Override
            public void onClick(View v) {
                if (musicBinder.getState() != STATE_STOP) {
                    musicBinder.stop();
                }
                if (height == 0 || width == 0) {
                    height = (int) v.getTag(R.id.music_found_height);
                    width = (int) v.getTag(R.id.music_found_width);
                }
                Log.e(TAG, "onClick: " + width + height);
                Surface surface = (Surface) v.getTag(R.id.music_found_surface);
                musicBinder.play((String) v.getTag(R.id.music_found_tag), surface, width, height);
            }
        });

//        surfaceView = v.findViewById(R.id.fragment_found_surface);
//        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
//            @Override
//            public void surfaceCreated(SurfaceHolder holder) {
//
//            }
//
//            @Override
//            public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
//                surfaceHolder = holder;
//                width = w;
//                height = h;
//            }
//
//            @Override
//            public void surfaceDestroyed(SurfaceHolder holder) {
//
//            }
//        });
//
//        surfaceView.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                musicBinder.play(getContext().getExternalFilesDir(null).getPath() + "/test.mp4", surfaceHolder.getSurface(), width, height);
//            }
//        });

        return v;
    }

    private void initView(View v) {
        recyclerView = v.findViewById(R.id.fragment_found_rv);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        list = new ArrayList<>();
        list.add(new FoundModel(getContext().getExternalFilesDir(null).getPath() + "/test.mp4", 11, 11, "111"));
        adapter = new FoundAdapter(list, getContext());
        recyclerView.setAdapter(adapter);
    }
}
