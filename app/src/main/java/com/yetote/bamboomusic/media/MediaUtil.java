package com.yetote.bamboomusic.media;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodecList;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

import androidx.annotation.NonNull;

import com.yetote.bamboomusic.util.WriteFile;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;

import static android.media.MediaCodecList.REGULAR_CODECS;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.media
 * @class describe
 * @time 2019/8/13 17:11
 * @change
 * @chang time
 * @class describe
 */
public class MediaUtil {
    static Map<String, String> map = new HashMap<>();
    private static MediaCodecList list = new MediaCodecList(REGULAR_CODECS);
    private static final String TAG = "MediaCodecSupport";
    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private MediaExtractor mediaExtractor;
    private WriteFile writeFile;
    private String pcmPath;
    private HandlerThread handlerThread;
    private HardwarePlayer hardwarePlayer;

    static {
        map.put("audio/mp4a-latm", "aac");
        map.put("audio/mpeg", "mp3");
        map.put("video/avc", "h264");
        map.put("video/hevc", "h265");
    }

    public static boolean hardwareSupport(String mutex) {
        Log.e(TAG, "isSupport: " + mutex + " ?");
        for (int i = 0; i < list.getCodecInfos().length; i++) {
            String[] support = list.getCodecInfos()[i].getSupportedTypes();
            list.getCodecInfos()[i].getCapabilitiesForType(support[0]);
            for (int j = 0; j < support.length; j++) {
                if (mutex.equals(map.get(support[j]))) {
                    Log.e(TAG, list.getCodecInfos()[i] + "支持: " + support[j]);
                    return true;
                }
            }
        }
        return false;
    }

    public void codec(String path) {
        if (mediaFormat == null) {
            initFmt(path);
        }
        mediaCodec.setCallback(new MediaCodec.Callback() {
            @Override
            public void onInputBufferAvailable(@NonNull MediaCodec codec, int index) {
                if (index >= 0) {
                    ByteBuffer inputBuffer = codec.getInputBuffer(index);
                    inputBuffer.clear();
                    int dataSize = mediaExtractor.readSampleData(inputBuffer, 0);
                    Log.e(TAG, "onInputBufferAvailable: 读取了" + dataSize + "字节");
                    if (dataSize < 0) {
                        codec.queueInputBuffer(index, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                        Log.e(TAG, "onInputBufferAvailable: 数据全部读取完毕");
                    } else {
                        codec.queueInputBuffer(index, 0, dataSize, 0, 0);
                        mediaExtractor.advance();
                    }
                }

            }

            @Override
            public void onOutputBufferAvailable(@NonNull MediaCodec codec, int index, @NonNull MediaCodec.BufferInfo info) {
                if (index >= 0) {
                    ByteBuffer outBuffer = codec.getOutputBuffer(index);
                    byte[] data = new byte[info.size];
                    outBuffer.position(info.offset);
                    outBuffer.get(data, 0, info.size);
                    outBuffer.clear();
//                    writeFile.write(data);
                    hardwarePlayer.pushData(data);
                    mediaCodec.releaseOutputBuffer(index, false);
                }
            }

            @Override
            public void onError(@NonNull MediaCodec codec, @NonNull MediaCodec.CodecException e) {
                Log.e(TAG, "onError: 解码发生异常" + e);
            }

            @Override
            public void onOutputFormatChanged(@NonNull MediaCodec codec, @NonNull MediaFormat format) {
                Log.e(TAG, "onOutputFormatChanged: fmt格式改变");
                hardwarePlayer.init(format);
            }
        }, new Handler(handlerThread.getLooper()));
        mediaCodec.start();
    }

    private void initFmt(String path) {
        try {
            mediaExtractor = new MediaExtractor();
            mediaExtractor.setDataSource(path);
            for (int i = 0; i < mediaExtractor.getTrackCount(); i++) {
                mediaFormat = mediaExtractor.getTrackFormat(i);
                String mime = mediaFormat.getString(MediaFormat.KEY_MIME);
                if (mime.startsWith("audio")) {
                    mediaExtractor.selectTrack(i);
                    mediaCodec = MediaCodec.createDecoderByType(mime);
                    mediaCodec.configure(mediaFormat, null, null, 0);
                    break;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public MediaUtil(Context context) {
        pcmPath = context.getExternalFilesDir(null).getPath() + "/test.pcm";
        writeFile = new WriteFile();
        writeFile.open(pcmPath);
        handlerThread = new HandlerThread("decodeThread");
        handlerThread.start();
        hardwarePlayer = new HardwarePlayer();
    }
}
