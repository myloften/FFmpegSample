#include <jni.h>
#include <LogUtils.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"

JNIEXPORT jint JNICALL
Java_com_loften_ffmpegsample_ffmpeglib_MainFFmpeg_decodeVideo(JNIEnv *env, jobject instance,
                                                              jobject surface, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    av_register_all();

    //FFMPEG解封装（flv，mp4，rmvb，avi）功能的结构体
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    //打开视频
    if(avformat_open_input(&pFormatCtx, path, NULL, NULL)!=0){
        LOGE("Couldn't open file:%s\n", path);
        return -1;
    }

    //检索流信息
    if(avformat_find_stream_info(pFormatCtx, NULL) < 0){
        LOGE("Coudln't find stream information.");
        return -1;
    }

    //找到第一个视频流
    int videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
           && videoStream < 0){
            videoStream = i;
        }
    }
    if(videoStream == -1){
        LOGE("Didn't find a video stream.");
        return -1;
    }

    //得到一个指向视频编解码器的上下文
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    //找到视频解码器
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL){
        LOGE("Codec not found.");
        return -1;
    }

    if(avcodec_open2(pCodecCtx,pCodec,NULL) < 0){
        LOGE("Couldn't not open codec.");
        return -1;// Could not open code
    }

    //获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    //获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    //设置nativewindow的缓冲区，width，height来控制缓冲区的大小（像素），buffer会进行相应的缩放来适配屏幕的大小
    ANativeWindow_setBuffersGeometry(nativeWindow,videoWidth,videoHeight,WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer windowBuffer;

    if(avcodec_open2(pCodecCtx,pCodec,NULL) < 0){
        LOGE("Couldn't not open codec.");
        return -1;
    }

    //分配视频帧
    AVFrame *pFrame = av_frame_alloc();

    //用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();
    if(pFrameRGBA == NULL || pFrame == NULL){
        LOGE("Couldn't allocate video frame.");
        return -1;
    }

    //确定所需的缓冲区大小和分配缓冲区
    //buffer中数据就是用于渲染的，且格式为RGBA
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA,pCodecCtx->width,pCodecCtx->height,1);

    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    //由于解码出来的帧格式不是RGBA的，在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int frameFinished;
    AVPacket packet;
    while (av_read_frame(pFormatCtx,&packet) >= 0){
        //判断是否这个视频流的包
        if(packet.stream_index == videoStream){
            //解码帧视频
            avcodec_decode_video2(pCodecCtx,pFrame,&frameFinished,&packet);

            //并不是decode一次就可以解码出一帧
            if(frameFinished){
                //锁定本地窗口缓冲区
                ANativeWindow_lock(nativeWindow, &windowBuffer,0);

                //格式转换
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                //获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
        av_packet_unref(&packet);
    }

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    env->ReleaseStringUTFChars(path_, path);
    return 0;
}
}
