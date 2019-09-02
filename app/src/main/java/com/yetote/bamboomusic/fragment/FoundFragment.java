package com.yetote.bamboomusic.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.yetote.bamboomusic.R;
import com.yetote.bamboomusic.adapter.FoundAdapter;
import com.yetote.bamboomusic.model.FoundModel;

import java.util.ArrayList;

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

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_found, null);
        initView(v);
        return v;
    }

    private void initView(View v) {
        recyclerView = v.findViewById(R.id.fragment_found_rv);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        list = new ArrayList<>();
//        list.add(new FoundModel("http://fs.mv.web.kugou.com/201909011024/dbd0fd13d6ec5e3ee7c58b7a4cf811b4/G139/M05/0A/0D/yw0DAFtrE_SAMm5CAMOFQ0-isyk330.mp4", 11, 11, "111"));
//        list.add(new FoundModel("http://fs.mv.web.kugou.com/201909011122/81a1ac007069681296fb8d15fe3f3445/G135/M01/10/01/xw0DAFtrkvOARUjvAyqky59_O0E064.mp4", 11, 11, "111"));
        list.add(new FoundModel(getContext().getExternalFilesDir(null).getPath() + "/test.avi", 11, 11, "111"));
        list.add(new FoundModel(getContext().getExternalFilesDir(null).getPath() + "/test.mp4", 11, 11, "111"));
        adapter = new FoundAdapter(list, getContext());
        recyclerView.setAdapter(adapter);
    }
}
