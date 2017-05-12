package com.loften.ffmpegsample.live555;

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

public class TestLive555Activity extends AppCompatActivity implements SurfaceHolder.Callback{
    private TextView tv_title;
    private Toolbar tool_bar;
    private SurfaceView surface_view;
    private SurfaceHolder mSurfaceHolder;
    private String path;

    public static void startActivity(Context context){
        Intent i = new Intent(context, TestLive555Activity.class);
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
        //mSurfaceHolder.addCallback(this);
        new Thread(new Runnable() {
            @Override
            public void run() {
                path = "rtsp://admin:admin@192.168.100.1:554/cam1/h264";
                MainLive555 mainLive555 = new MainLive555();
                mainLive555.startRTSPClient(mSurfaceHolder.getSurface(), path);
            }
        }).start();
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
                path = "rtsp://admin:admin@192.168.100.1:554/cam1/h264";
                MainLive555 mainLive555 = new MainLive555();
                mainLive555.startRTSPClient(mSurfaceHolder.getSurface(), path);
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
