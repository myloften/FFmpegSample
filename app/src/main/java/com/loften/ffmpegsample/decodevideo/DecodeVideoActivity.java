package com.loften.ffmpegsample.decodevideo;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import com.loften.ffmpegsample.R;
import com.loften.ffmpegsample.ffmpeglib.MainFFmpeg;

public class DecodeVideoActivity extends AppCompatActivity implements SurfaceHolder.Callback{

    private TextView tv_title;
    private Toolbar tool_bar;
    private SurfaceView surface_view;
    private SurfaceHolder mSurfaceHolder;
    private String path;

    public static void startActivity(Context context, String path){
        Intent i = new Intent(context, DecodeVideoActivity.class);
        i.putExtra("path", path);
        context.startActivity(i);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_decode_video);
        initView();
        initDatas();
    }

    private void initDatas() {
        tv_title.setText("视频播放");
        mSurfaceHolder = surface_view.getHolder();
        mSurfaceHolder.addCallback(this);
        path = getIntent().getStringExtra("path");
    }

    private void initView() {
        tv_title = (TextView) findViewById(R.id.tv_title);
        tool_bar = (Toolbar) findViewById(R.id.tool_bar);
        surface_view = (SurfaceView) findViewById(R.id.surface_view);
        setSupportActionBar(tool_bar);
        getSupportActionBar().setDisplayShowTitleEnabled(false);
        tool_bar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                //path = Environment.getExternalStorageDirectory().getPath()+"/film/in.mp4";
                MainFFmpeg mainFFmpeg = new MainFFmpeg();
                mainFFmpeg.decodeVideo(mSurfaceHolder.getSurface(), path);
            }
        }).start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
