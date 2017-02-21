#include <jni.h>
#include <LogUtils.h>
#include <unistd.h>

#define MAX_AUDIO_FRAME_SIZE  192000

extern "C"{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"

JNIEXPORT void JNICALL
Java_com_loften_ffmpegsample_ffmpeglib_MainFFmpeg_decodeAudio(JNIEnv *env, jobject instance,
        jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    av_register_all();

    //FFMPEG解封装（flv，mp4，rmvb，avi）功能的结构体
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    //打开video
    if(avformat_open_input(&pFormatCtx, path, NULL, NULL) != 0){
        LOGE("Couldn't open file:%s\n", path);
        return;
    }

    //检索流信息
    if(avformat_find_stream_info(pFormatCtx, NULL) < 0 ){
        LOGE("Couldn't find stream information.");
        return;
    }

    //找到音频流索引
    int audioIndex = -1;
    for (int i = 0; i < pFormatCtx->nb_streams ; ++i) {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audioIndex = i;
            break;
        }
    }
    if(audioIndex == -1){
        LOGE("Didn't find a audio stream.");
        return;
    }

    //得到一个指向视频编解码器的上下文
    AVCodecContext *pCodecCtx = pFormatCtx->streams[audioIndex]->codec;

    //找到视频解码器
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL){
        LOGE("Codec not found.");
        return;
    }

    //打开解码器
    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
        LOGE("Couldn't not open codec.");
        return;
    }

    //拿到数据帧，开始解码
    AVFrame *avFrame = av_frame_alloc();
    int got_frame_ptr;
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));

    struct SwrContext *swrCtx = swr_alloc();
    //输出的缓冲区大小
    uint8_t *outBuffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE);

    //输出声道布局
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    int out_sample_rate = 44100;
    //输入的声道布局
    int64_t in_ch_layout = pCodecCtx->channel_layout;
    enum AVSampleFormat in_sample_fmt = pCodecCtx->sample_fmt;
    int in_sample_rate = pCodecCtx->sample_rate;
    int nb_channels = av_get_channel_layout_nb_channels(out_ch_layout);
    //设置转换的参数选项
    swr_alloc_set_opts(swrCtx, out_ch_layout, out_sample_fmt, out_sample_rate,
                        in_ch_layout, in_sample_fmt, in_sample_rate, 0, NULL);
    swr_init(swrCtx);

    //调用createAudio
    jclass player_class = env->GetObjectClass(instance);
    jmethodID createAudioMID = env->GetMethodID(player_class, "createAudio", "()Landroid/media/AudioTrack;");

    jobject audioTraker = env->CallObjectMethod(instance, createAudioMID);

    //调用play
    jclass audio_class = env->GetObjectClass(audioTraker);
    jmethodID playMID = env->GetMethodID(audio_class, "play", "()V");
    env->CallVoidMethod(audioTraker, playMID);
    jmethodID writeMID = env->GetMethodID(audio_class, "write", "([BII)I");

    while (av_read_frame(pFormatCtx, avPacket) >= 0){
        //解音频的压缩数据
        if(avPacket->stream_index == audioIndex){
            int len = avcodec_decode_audio4(pCodecCtx, avFrame, &got_frame_ptr, avPacket);
            if(len < 0){
                LOGD("解码完成");
            }
            //frame-->16bit PCM数据，写入文件
            if(got_frame_ptr){
                swr_convert(swrCtx, &outBuffer, MAX_AUDIO_FRAME_SIZE,
                            (const uint8_t **)avFrame->data,avFrame->nb_samples);
                int bufferSize = av_samples_get_buffer_size(NULL, nb_channels,
                                                        avFrame->nb_samples, out_sample_fmt, 1);
                //调用write
                jbyteArray byteArray = env->NewByteArray(bufferSize);
                jbyte *byteElement = env->GetByteArrayElements(byteArray,NULL);

                //out_buffer的数据复制到sampe_bytep
                memcpy(byteElement, outBuffer, bufferSize);
                env->ReleaseByteArrayElements(byteArray,byteElement,0);
                env->CallIntMethod(audioTraker, writeMID, byteArray, 0, bufferSize);

                //释放局部引用
                env->DeleteLocalRef(byteArray);
                usleep(1000 * 16);
            }
        }
        av_free_packet(avPacket);
    }
    swr_free(&swrCtx);
    av_frame_free(&avFrame);
    av_free(outBuffer);
    avformat_free_context(pFormatCtx);
    avcodec_free_context(&pCodecCtx);
    env->ReleaseStringUTFChars(path_, path);
}
}