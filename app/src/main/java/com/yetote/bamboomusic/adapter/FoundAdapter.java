package com.yetote.bamboomusic.adapter;

import android.content.Context;
import android.media.Image;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.yetote.bamboomusic.R;
import com.yetote.bamboomusic.model.FoundModel;

import java.util.ArrayList;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.adapter
 * @class describe
 * @time 2019/9/2 14:17
 * @change
 * @chang time
 * @class describe
 */
public class FoundAdapter extends RecyclerView.Adapter {
    private ArrayList<FoundModel> list;
    private Context context;
    private static final String TAG = "FoundAdapter";
    private RecyclerViewItemClickListener recyclerViewItemClickListener;

    public void setRecyclerViewItemClickListener(RecyclerViewItemClickListener recyclerViewItemClickListener) {
        this.recyclerViewItemClickListener = recyclerViewItemClickListener;
    }

    public FoundAdapter(ArrayList<FoundModel> list, Context context) {
        this.list = list;
        this.context = context;
    }


    class MyViewHolder extends RecyclerView.ViewHolder {
        private SurfaceView surfaceView;
        private TextView tag;
        private Button praise, discuss;
        private ImageView fullIv, start;

        public MyViewHolder(@NonNull View itemView) {
            super(itemView);
            surfaceView = itemView.findViewById(R.id.rv_music_found_item_surface);
            tag = itemView.findViewById(R.id.rv_music_found_item_tag);
            praise = itemView.findViewById(R.id.rv_music_found_item_praise);
            start = itemView.findViewById(R.id.rv_music_found_item_start);
            fullIv = itemView.findViewById(R.id.rv_music_found_item_full);
        }

        public ImageView getFullIv() {
            return fullIv;
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

        public ImageView getStart() {
            return start;
        }
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(context).inflate(R.layout.rv_music_found_layout, parent, false);
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
                // TODO: 2019/9/2  需要使用广播将销毁消息传递给activity
            }
        });
        vh.getStart().setOnClickListener(v -> recyclerViewItemClickListener.onItemClick(v, vh.itemView, position));
        vh.getFullIv().setOnClickListener(v -> recyclerViewItemClickListener.onItemClick(v, vh.itemView, position));
    }

    @Override
    public int getItemCount() {
        return list.size();
    }


}
