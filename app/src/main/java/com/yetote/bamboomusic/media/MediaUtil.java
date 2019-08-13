package com.yetote.bamboomusic.media;

import android.media.MediaCodecList;
import android.util.Log;

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

    static {
        map.put("audio/mp4a-latm", "aac");
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
}
