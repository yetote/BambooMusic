package com.yetote.bamboomusic.adapter;

import android.view.View;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.adapter
 * @class describe
 * @time 2019/8/15 13:31
 * @change
 * @chang time
 * @class describe
 */
public interface RecyclerViewItemClickListener {
    /**
     * item的点击事件
     *
     * @param v 点击的view
     */
    void onClick(View v);

    void onItemClick(View v, View parent, int pos);
}
