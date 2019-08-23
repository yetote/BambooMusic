package com.yetote.bamboomusic.adapter;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.yetote.bamboomusic.R;
import com.yetote.bamboomusic.media.MusicService;
import com.yetote.bamboomusic.model.FoundModel;

import java.util.ArrayList;

import static android.content.Context.BIND_AUTO_CREATE;
import static com.yetote.bamboomusic.media.MusicService.STATE_STOP;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.adapter
 * @class describe
 * @time 2019/8/15 13:14
 * @change
 * @chang time
 * @class describe
 */
public class FoundAdapter extends RecyclerView.Adapter {
    private ArrayList<FoundModel> list;
    private Context context;
    public static final int PATH_TAG = 1;
    RecyclerViewItemClickListener itemClickListener;
    private static final String TAG = "FoundAdapter";
    private MusicService.MusicBinder musicBinder;
    private SeekBar seekBar;
    private Surface surface;
    private int width, height;
    private boolean isPlaying;
    private boolean isPausing;
    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {

            musicBinder = (MusicService.MusicBinder) service;
            musicBinder.setOnPrepareCallback((prepare, totalTime) -> {
                if (totalTime != 0) {
                    seekBar.setMax(totalTime);
                }
                musicBinder.play(surface, width, height);
                isPlaying = true;
            });

            musicBinder.setPlayCallback(currentTime -> {
                seekBar.setProgress(currentTime);
            });
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    public void setItemClickListener(RecyclerViewItemClickListener itemClickListener) {
        this.itemClickListener = itemClickListener;
    }

    public FoundAdapter(ArrayList<FoundModel> list, Context context) {
        this.list = list;
        this.context = context;
        Intent musicService = new Intent(context, MusicService.class);
        context.bindService(musicService, serviceConnection, BIND_AUTO_CREATE);
    }

    class MyViewHolder extends RecyclerView.ViewHolder {
        private SurfaceView surfaceView;
        private TextView tag;
        private Button praise, discuss;

        public MyViewHolder(@NonNull View itemView) {
            super(itemView);
            surfaceView = itemView.findViewById(R.id.rv_music_found_item_surface);
            tag = itemView.findViewById(R.id.rv_music_found_item_tag);
            praise = itemView.findViewById(R.id.rv_music_found_item_praise);
        }

        public SurfaceView getSurfaceView() {
            return surfaceView;
        }

        public TextView getTag() {
            return tag;
        }

        public Button getPraise() {
            return praise;
        }

        public Button getDiscuss() {
            return discuss;
        }
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(context).inflate(R.layout.rv_music_found_layout, parent, false);
        v.setOnClickListener(v1 -> {
            if (!isPlaying) {
                if (musicBinder.getState() != STATE_STOP) {
                    musicBinder.stop();
                }
                seekBar = v1.findViewById(R.id.rv_music_found_item_seek);
                if (surface != null) {
                    surface.release();
                }
                if (width == 0 || height == 0) {
                    height = (int) v1.getTag(R.id.music_found_tag_height);
                    width = (int) v1.getTag(R.id.music_found_tag_width);
                }
                surface = (Surface) v1.getTag(R.id.music_found_tag_surface);
                musicBinder.prepare((String) v1.getTag(R.id.music_found_tag_path));
            } else {
                if (isPausing) {
                    musicBinder.resume();
                    isPausing = false;
                } else {
                    isPausing = true;
                    musicBinder.pause();
                }
            }


            seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                    if (isPlaying) {
                        musicBinder.pause();
                    }
                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                    if (isPlaying) {
                        musicBinder.seek(seekBar.getProgress());
                        musicBinder.resume();
                    }
                }
            });
        });
        return new MyViewHolder(v);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        MyViewHolder vh = (MyViewHolder) holder;
        vh.itemView.setTag(R.id.music_found_tag_path, list.get(position).getPath());
        vh.getTag().setText(list.get(position).getTag());
        vh.getPraise().setText(list.get(position).getPraiseNum() + "");
        vh.getSurfaceView().getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Log.e(TAG, "surfaceCreated: 创建");
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                vh.itemView.setTag(R.id.music_found_tag_width, width);
                vh.itemView.setTag(R.id.music_found_tag_height, height);
                vh.itemView.setTag(R.id.music_found_tag_surface, holder.getSurface());
                vh.itemView.setTag(R.id.music_found_tag_position, position);
                Log.e(TAG, "surfaceChanged: ");
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.e(TAG, "surfaceDestroyed: 销毁");
                musicBinder.stop();
            }
        });
    }

    @Override
    public int getItemCount() {
        return list.size();
    }

    @Override
    public void onViewRecycled(@NonNull RecyclerView.ViewHolder holder) {
        super.onViewRecycled(holder);
//        musicBinder.stop();
    }
}
