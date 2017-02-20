package com.loften.ffmpegsample;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.TextView;

import com.loften.ffmpegsample.video.VideoActivity;

public class MainActivity extends AppCompatActivity {

    private TextView tv_title;
    private Toolbar tool_bar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
        /*String path = Environment.getExternalStorageDirectory().getPath()+"/film/in.mp4";
        MainFFmpeg mainFFmpeg = new MainFFmpeg();
        mainFFmpeg.screencaps(path);*/
        initEvents();

    }

    private void initEvents() {
        findViewById(R.id.btn_decode_video).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, VideoActivity.class));
            }
        });
    }


    private void initView() {
        tv_title = (TextView) findViewById(R.id.tv_title);
        tool_bar = (Toolbar) findViewById(R.id.tool_bar);
        setSupportActionBar(tool_bar);
        getSupportActionBar().setDisplayShowTitleEnabled(false);
        getSupportActionBar().setDisplayHomeAsUpEnabled(false);
        tv_title.setText("FFmpegSample");
    }
}
