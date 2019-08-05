package com.yetote.bamboomusic;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.yetote.bamboomusic.myview.MusicProgressButton;

public class MainActivity extends AppCompatActivity {
    MusicProgressButton musicProgressButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        musicProgressButton = findViewById(R.id.main_musicProgress_btn);
        musicProgressButton.changeState(MusicProgressButton.STATE_PROGRESS);
        musicProgressButton.setTotalTime(10);
        musicProgressButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (musicProgressButton.getPlayState() == MusicProgressButton.STATE_PLAYING || musicProgressButton.getPlayState() == MusicProgressButton.STATE_PROGRESS) {
                    musicProgressButton.changeState(MusicProgressButton.STATE_STOP);
                } else {
                    musicProgressButton.changeState(MusicProgressButton.STATE_PROGRESS);
                }
            }
        });
    }

}
