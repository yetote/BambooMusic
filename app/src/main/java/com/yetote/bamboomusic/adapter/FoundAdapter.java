package com.yetote.bamboomusic.adapter;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
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

    public void setItemClickListener(RecyclerViewItemClickListener itemClickListener) {
        this.itemClickListener = itemClickListener;
    }

    public FoundAdapter(ArrayList<FoundModel> list, Context context) {
        this.list = list;
        this.context = context;
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
        v.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                v.setTag(R.id.music_found_width, v.findViewById(R.id.rv_music_found_item_surface).getWidth());
                v.setTag(R.id.music_found_height, v.findViewById(R.id.rv_music_found_item_surface).getHeight());
                v.setTag(R.id.music_found_surface, ((SurfaceView)v.findViewById(R.id.rv_music_found_item_surface)).getHolder().getSurface());
                itemClickListener.onClick(v);

            }
        });
        return new MyViewHolder(v);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        MyViewHolder vh = (MyViewHolder) holder;
        vh.itemView.setTag(R.id.music_found_tag, list.get(position).getPath());
        vh.getTag().setText(list.get(position).getTag());
        vh.getPraise().setText(list.get(position).getPraiseNum() + "");

    }

    @Override
    public int getItemCount() {
        return list.size();
    }
}
