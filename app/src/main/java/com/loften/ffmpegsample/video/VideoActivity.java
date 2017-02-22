package com.loften.ffmpegsample.video;

import android.graphics.Bitmap;
import android.media.ThumbnailUtils;
import android.os.AsyncTask;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.TextView;

import com.loften.ffmpegsample.R;

public class VideoActivity extends AppCompatActivity {

    private TextView tv_title;
    private Toolbar tool_bar;
    private RecyclerView rv_video;
    private VideoAdapter mAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        initView();

        initDatas();
    }

    private void initDatas() {
        tv_title.setText("本地视频");
        mAdapter = new VideoAdapter(this, new VideoProvider(this).getList());
        rv_video.setLayoutManager(new LinearLayoutManager(this));
        rv_video.setAdapter(mAdapter);
        new AsyncTask<Void,Void,Void>(){
            @Override
            protected Void doInBackground(Void... params) {
                loadImage();
                return null;
            }

            @Override
            protected void onPostExecute(Void aVoid) {
                super.onPostExecute(aVoid);
                mAdapter.notifyDataSetChanged();
            }
        }.execute();
    }

    private void initView() {
        tv_title = (TextView) findViewById(R.id.tv_title);
        tool_bar = (Toolbar) findViewById(R.id.tool_bar);
        rv_video = (RecyclerView) findViewById(R.id.rv_video);
        setSupportActionBar(tool_bar);
        getSupportActionBar().setDisplayShowTitleEnabled(false);
        tool_bar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
    }


    private void loadImage(){
        for (int i = 0; i < mAdapter.getDatas().size(); i++) {
            Bitmap img = getVideoThumbnail(mAdapter.getDatas().get(i).getPath(),
                    120, 120, MediaStore.Video.Thumbnails.MINI_KIND);
            mAdapter.getDatas().get(i).setImg(img);
        }


    }

    //获取视频缩略图
    private Bitmap getVideoThumbnail(String videoPath, int width , int height, int kind){
        Bitmap bitmap = null;
        bitmap = ThumbnailUtils.createVideoThumbnail(videoPath, kind);
        bitmap = ThumbnailUtils.extractThumbnail(bitmap, width, height, ThumbnailUtils.OPTIONS_RECYCLE_INPUT);
        return bitmap;
    }
}
