package com.yetote.bamboomusic.util;

import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;

/**
 * @author ether QQ:503779938
 * @name HardwareEncodeDemo
 * @class name：com.example.hardwareencodedemo
 * @class describe
 * @time 2019/5/14 11:38
 * @change
 * @chang time
 * @class describe
 */
public class WriteFile {
    private FileOutputStream outputStream;
    private FileChannel fileChannel;
    private static final String TAG = "WriteFile";
    private ByteBuffer byteBuffer;

    public WriteFile open(String path) {
        File file = new File(path);

        if (!file.exists()) {
            file.getParentFile().mkdir();
        }
        try {
            outputStream = new FileOutputStream(file);
            fileChannel = outputStream.getChannel();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        return this;
    }


    public WriteFile write(ByteBuffer byteBuffer) {
//        Log.e(TAG, "write: " + byteBuffer.limit());
        if (fileChannel == null) {
            Log.e(TAG, "write: 未打开channel");
            return this;
        }
        if (byteBuffer.position() != 0) {
            byteBuffer.flip();
        }

        try {
            fileChannel.write(byteBuffer);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return this;

    }

    public void close() {
        try {
            fileChannel.close();
            outputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public WriteFile write(byte[] bytes) {
        byteBuffer = ByteBuffer.allocate(bytes.length).order(ByteOrder.nativeOrder());
        byteBuffer.clear();
        byteBuffer.put(bytes);
        return write(byteBuffer);
    }
}
