package com.loften.ffmpegsample.ffmpeglib;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

/**
 * Created by asus on 2017/2/17.
 */

public class MainFFmpeg {
    static {
        System.loadLibrary("ffmpeg-lib");
    }

    //截图
    public native int screencaps(String input_url);

    //编码视频 -->  Android+FFmpeg+ANativeWindow视频解码播放
    public native int decodeVideo(Object surface, String path);

    //音频解码 -->  得到音频采样数据并用AudioTraker播放
    public native void decodeAudio(String path);
    //供原生代码调用获取AudioTraker对象
    public AudioTrack createAudio() {

        int sampleRateInHz = 44100;
        int channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        int minBufferSize = AudioTrack.getMinBufferSize(sampleRateInHz,
                channelConfig, audioFormat);

        AudioTrack audio = new AudioTrack(AudioManager.STREAM_MUSIC, // 指定流的类型
                sampleRateInHz, // 设置音频数据的采样率 32k，如果是44.1k就是44100
                channelConfig, // 设置输出声道为双声道立体声，而CHANNEL_OUT_MONO类型是单声道
                audioFormat, // 设置音频数据块是8位还是16位，这里设置为16位。好像现在绝大多数的音频都是16位的了
                minBufferSize, AudioTrack.MODE_STREAM // 设置模式类型，在这里设置为流类型，另外一种MODE_STATIC貌似没有什么效果
        );
        // audio.play(); // 启动音频设备，下面就可以真正开始音频数据的播放了
        return audio;
    }

}
