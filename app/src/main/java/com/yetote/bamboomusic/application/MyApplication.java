package com.yetote.bamboomusic.application;

import android.app.Application;

import com.tencent.bugly.crashreport.CrashReport;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.application
 * @class describe
 * @time 2019/8/6 14:27
 * @change
 * @chang time
 * @class describe
 */
public class MyApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        CrashReport.initCrashReport(getApplicationContext(), "622edf22cb", true);
    }
}
