package com.yetote.bamboomusic.media;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioTrack;
import android.media.MediaFormat;
import android.util.Log;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import static android.media.AudioFormat.CHANNEL_OUT_STEREO;
import static android.media.AudioFormat.ENCODING_PCM_16BIT;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.media
 * @class describe
 * @time 2019/8/14 10:20
 * @change
 * @chang time
 * @class describe
 */
public class HardwarePlayer {
    private AudioTrack audioTrack;
    private boolean prepare;
    private Thread playThread;
    private boolean isPlaying;
    private static final String TAG = "HardwarePlayer";

    public HardwarePlayer() {
        dataQueue = new LinkedBlockingQueue<>();
        playThread = new Thread(() -> {
            while (isPlaying) {
                play();
            }
        });
    }

    private BlockingQueue<byte[]> dataQueue;

    void init(MediaFormat mediaFormat) {
        int sampleRate = mediaFormat.getInteger("sample-rate");
        int channelCount = mediaFormat.getInteger("channel-count");
        int minBufferSize = AudioTrack.getMinBufferSize(sampleRate, channelCount, ENCODING_PCM_16BIT);
        audioTrack = new AudioTrack.Builder()
                .setAudioAttributes(new AudioAttributes.Builder()
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .setUsage(AudioAttributes.USAGE_MEDIA).build())
                .setAudioFormat(new AudioFormat.Builder().setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                        .setSampleRate(sampleRate)
                        .setChannelMask(CHANNEL_OUT_STEREO)
                        .build())
                .setBufferSizeInBytes(minBufferSize)
                .build();
        isPlaying = true;
        playThread.start();
    }

    private void play() {
        if (!dataQueue.isEmpty()) {
            Log.e(TAG, "play: " + "开始播放");
            try {
                byte[] data = dataQueue.take();
                audioTrack.write(data, 0, data.length);
                audioTrack.play();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }


    public void pushData(byte[] data) {
        try {
            dataQueue.put(data);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
