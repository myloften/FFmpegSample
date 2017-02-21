package com.loften.ffmpegsample.decodeaudio;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.loften.ffmpegsample.R;
import com.loften.ffmpegsample.ffmpeglib.MainFFmpeg;


public class DecodeAudioActivity extends AppCompatActivity {

    private String path;
    private TextView tv_title;
    private Toolbar tool_bar;

    public static void startActivity(Context context, String path) {
        Intent i = new Intent(context, DecodeAudioActivity.class);
        i.putExtra("path", path);
        context.startActivity(i);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_decode_audio);
        initView();
        initData();
    }

    private void initData() {
        tv_title.setText("解码音频");
        path = getIntent().getStringExtra("path");
        new Thread(new Runnable() {
            @Override
            public void run() {
                MainFFmpeg mainFFmpeg = new MainFFmpeg();
                mainFFmpeg.decodeAudio(path);
            }
        }).start();
    }

    private void initView() {
        tv_title = (TextView) findViewById(R.id.tv_title);
        tool_bar = (Toolbar) findViewById(R.id.tool_bar);
        setSupportActionBar(tool_bar);
        getSupportActionBar().setDisplayShowTitleEnabled(false);
        tool_bar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
    }
}
