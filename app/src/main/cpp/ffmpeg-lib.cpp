#include <jni.h>
#include <LogUtils.h>

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
    FILE *pFile;
    char szFilename[42];
    int  y;

    //打开文件
    sprintf(szFilename, "/storage/emulated/0/film/frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");

    if(pFile==NULL)
        return;


    // 写入 header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // 写入 pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);


    // 关闭文件
    fclose(pFile);
}

JNIEXPORT jint JNICALL
Java_com_loften_ffmpegsample_ffmpeglib_MainFFmpeg_screencaps(JNIEnv *env, jobject instance,
                                                             jstring input_url_) {
    const char *url = env->GetStringUTFChars(input_url_, 0);
    AVFormatContext   *pFormatCtx = NULL;
    int               i, videoStream;
    AVCodecContext    *pCodecCtxOrig = NULL;
    AVCodecContext    *pCodecCtx = NULL;
    AVCodec           *pCodec = NULL;
    AVFrame           *pFrame = NULL;
    AVFrame           *pFrameRGB = NULL;
    AVPacket          packet;
    int               frameFinished;
    int               numBytes;
    uint8_t           *buffer = NULL;
    struct SwsContext *sws_ctx = NULL;

    av_register_all();

    //打开视频文件
    if(avformat_open_input(&pFormatCtx, url, NULL, NULL)!=0){
        LOGE("Couldn't open file:%s\n",url);
        return -1;
    }

    //检索流信息
    if(avformat_find_stream_info(pFormatCtx, NULL)<0){
        LOGE("Couldn't find stream information");
        return -1;
    }

    av_dump_format(pFormatCtx,0,url,0);

    //找到第一个视频流
    videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams ; ++i) {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStream = i;
            break;
        }
    }
    if(videoStream==-1){
        LOGE("Didn't find a video stream");
        return -1;
    }

    //得到一个指向视频编解码器的上下文
    pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
    //找到视频解码器
    pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if(pCodec==NULL){
        fprintf(stderr,"Unsupported codec!\n");
        LOGE("Codec not found");
        return -1;
    }

    //复制上下文
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(avcodec_copy_context(pCodecCtx,pCodecCtxOrig)!=0){
        fprintf(stderr, "Couldn't copy codec context");
        LOGE("Error copying codec context");
        return -1;
    }

    //打开codec
    if(avcodec_open2(pCodecCtx,pCodec,NULL)<0){
        LOGE("Could not open codec");
        return -1;
    }

    pFrame = av_frame_alloc();

    //分配一个 AVFrame 结构体
    pFrameRGB = av_frame_alloc();
    if(pFrameRGB == NULL){
        return -1;
    }

    //确定所需的缓冲区大小和分配缓冲区
    numBytes=avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width,
                                pCodecCtx->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    //分配适当的缓冲在pFrameRGB图像平面
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
                   pCodecCtx->width, pCodecCtx->height);

    //由于解码出来的帧格式不是RGBA的，在渲染之前需要进行格式转换
    sws_ctx = sws_getContext(pCodecCtx->width,
                             pCodecCtx->height,
                             pCodecCtx->pix_fmt,
                             pCodecCtx->width,
                             pCodecCtx->height,
                             AV_PIX_FMT_RGB24,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL
    );

    //读取帧并保存第一到第五帧到磁盘
    i = 0;
    while (av_read_frame(pFormatCtx, &packet) >=0 ){
        if(packet.stream_index == videoStream){
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            if(frameFinished){
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGB->data, pFrameRGB->linesize);

                if(++i<=5)
                    SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height,
                              i);
            }
        }
        av_free_packet(&packet);
    }

    av_free(buffer);
    av_frame_free(&pFrameRGB);

    av_frame_free(&pFrame);

    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    avformat_close_input(&pFormatCtx);

    env->ReleaseStringUTFChars(input_url_, url);
    return 0;
}
}