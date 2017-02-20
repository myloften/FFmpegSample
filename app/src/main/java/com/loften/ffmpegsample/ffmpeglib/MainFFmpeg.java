package com.loften.ffmpegsample.ffmpeglib;

/**
 * Created by asus on 2017/2/17.
 */

public class MainFFmpeg {
    static {
        System.loadLibrary("ffmpeg-lib");
    }

    //截图
    public native int screencaps(String input_url);

    //编码视频
    public native int decodeVideo(Object surface, String path);
}
