
#include "stdafx.h"
#include <jni.h>
#include <string>
#include <thread>
#include <memory>
#include <android/log.h>

using namespace std;

#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"TAG",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"TAG",FORMAT,##__VA_ARGS__);

AVFormatContext *inputContext = nullptr;
AVFormatContext *outputContext = nullptr;
int64_t  lastReadPacktTime;

static int interrupt_cb(void *ctx)
{
    int timeout = 10;
    if (av_gettime() - lastReadPacktTime > timeout *1000 * 1000){
        return -1;
    }
    return 0;
}
int OpenInput(string inputUrl){
    //輸入上下文 存儲音視頻信息
    inputContext = avformat_alloc_context();
    lastReadPacktTime = av_gettime();
    inputContext->interrupt_callback.callback = interrupt_cb;
    int ret = avformat_open_input(&inputContext,inputUrl.c_str(), nullptr, nullptr);
    if (ret < 0){
        av_log(NULL,AV_LOG_ERROR,"Input file open input failed\n");

        return ret;
    }
    //查找新視頻關鍵信息
    ret = avformat_find_stream_info(inputContext, nullptr);
    if (ret < 0){
        av_log(NULL,AV_LOG_ERROR,"Find input file stream inform failed\n");
    } else {
        av_log(NULL,AV_LOG_ERROR,"Open input file %s success\n",inputUrl.c_str());
    }
    return ret;
}
int OpenOutput(string outUrl){
    int ret = avformat_alloc_output_context2(&outputContext, nullptr,"mpegts",outUrl.c_str());
    if(ret < 0)
    {
        goto Error;
    }
    ret = avio_open2(&outputContext->pb, outUrl.c_str(), AVIO_FLAG_READ_WRITE,nullptr, nullptr);
    if(ret < 0)
    {
        goto Error;
    }
    for(int i = 0; i < inputContext->nb_streams; i++)
    {
        AVStream * stream = avformat_new_stream(outputContext, nullptr);
        ret = avcodec_copy_context(stream->codec, inputContext->streams[i]->codec);
        if(ret < 0)
        {
            goto Error;
        }
    }
    ret = avformat_write_header(outputContext, nullptr);
    if(ret < 0)
    {
        goto Error;
    }
    return ret ;
    Error:
    if(outputContext)
    {
        for(int i = 0; i < outputContext->nb_streams; i++)
        {
            avcodec_close(outputContext->streams[i]->codec);
        }
        avformat_close_input(&outputContext);
    }
    return ret ;
}
shared_ptr<AVPacket> ReadPacketFromSource()
{
    shared_ptr<AVPacket> packet(static_cast<AVPacket*>(av_malloc(sizeof(AVPacket))), [&](AVPacket *p) { av_free_packet(p); av_freep(&p);});
    av_init_packet(packet.get());
    lastReadPacktTime = av_gettime();
    int ret = av_read_frame(inputContext, packet.get());
    if(ret >= 0)
    {
        return packet;
    }
    else
    {
        return nullptr;
    }
}
int WritePacket(std::shared_ptr<AVPacket> packet)
{
    auto inputStream = inputContext->streams[packet->stream_index];
    auto outputStream = outputContext->streams[packet->stream_index];
    av_packet_rescale_ts(packet.get(), inputStream->time_base, outputStream->time_base);
    return av_interleaved_write_frame(outputContext, packet.get());
}
void Init(){
    av_register_all();
    avfilter_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_ERROR);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_cnr_ffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    av_register_all();
    avformat_network_init();
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    int ret = avformat_open_input(&pFormatCtx, "", NULL, NULL);
    if (ret != 0) {
        char errorbuf[1024] = {0};
        av_make_error_string(errorbuf, 1024, ret);
        LOGE("%s,%d,%s", "无法打开输入视频文件", ret, errorbuf);
    } else {
        LOGI("%s,%d,%d", "视频长度：", ret, pFormatCtx->duration);
    }
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_cnr_ffmpeg_MainActivity_avPlayerJNI(JNIEnv *env, jobject instance, jstring str_) {
    const char *str = env->GetStringUTFChars(str_, 0);


    Init();
    int ret = OpenInput(str);
    if (ret >= 0){
        ret = OpenOutput("/storage/emulated/0/Android/data/com.cnr.ffmpeg/cache/test1.ts");
    }
    while (true){
        auto  packet = ReadPacketFromSource();
        if (packet){
            ret = WritePacket(packet);
            if (ret >= 0){
                LOGI("WritePacket Success!")
            }else {
                LOGE("WritePacket failed!")
            }
        }
    }

    env->ReleaseStringUTFChars(str_, str);
}