package com.loften.ffmpegsample.video;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.loften.ffmpegsample.R;
import com.loften.ffmpegsample.decodevideo.DecodeVideoActivity;

import java.util.List;

public class VideoAdapter extends RecyclerView.Adapter<VideoAdapter.VideoViewHolder> {

    private List<VideoInfo> mDatas;
    private Context mContext;

    public VideoAdapter(Context context, List<VideoInfo> datas) {
        this.mContext = context;
        this.mDatas = datas;
    }

    @Override
    public VideoViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(mContext).inflate(R.layout.item_video, parent, false);
        VideoViewHolder holder = new VideoViewHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(VideoViewHolder holder, final int position) {
        holder.video_title.setText(mDatas.get(position).getTitle());
        holder.video_time.setText(mDatas.get(position).getDuration()/1000+"");
        holder.rootView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                DecodeVideoActivity.startActivity(mContext, mDatas.get(position).getPath());
                //DecodeAudioActivity.startActivity(mContext, mDatas.get(position).getPath());
            }
        });
    }

    @Override
    public int getItemCount() {
        return mDatas.size();
    }

    public static class VideoViewHolder extends RecyclerView.ViewHolder {
        public View rootView;
        public ImageView video_img;
        public TextView video_title;
        public TextView video_time;

        public VideoViewHolder(View rootView) {
            super(rootView);
            this.rootView = rootView;
            this.video_img = (ImageView) rootView.findViewById(R.id.video_img);
            this.video_title = (TextView) rootView.findViewById(R.id.video_title);
            this.video_time = (TextView) rootView.findViewById(R.id.video_time);
        }

    }
}
