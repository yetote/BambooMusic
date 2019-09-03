package com.yetote.bamboomusic.util;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.util
 * @class describe
 * @time 2019/8/12 16:39
 * @change
 * @chang time
 * @class describe
 */
public class TextUtil {
    public static String time2Str(int time) {
        String timeS = "";
        int min = time / 60;
        int sec = time % 60;
        if (min < 10) {
            timeS += ("0" + min);
        } else {
            timeS += min;
        }
        timeS += ":";
        if (sec < 10) {
            timeS += ("0" + sec);
        } else {
            timeS += sec;
        }
        return timeS;
    }
}
