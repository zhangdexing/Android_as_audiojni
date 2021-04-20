/*
 * @Copyright (C), 2020 : YBS
 * @内容简述:
 * @version:
 * @Author: kemp
 * @LastEditors: kemp
 * @LastEditTime: 2021-04-15 15:08:51
 */
#include "include/asoundlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include "unistd.h"

#include "ybsHalAlsa.h"
#include "YbsFrameQueue.h"

#include "ybs_mutl_ssp-master/dsplib/ybs_mutl_ssp.h"
#include "ybs_mutl_ssp-master/dsplib/overallContext.h"

int vadflag = 0;


#include "android/log.h"
static const char *TAG="ALSA";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

struct audiomsg{
    unsigned int size;
    uint8_t buf[65536];
};

int capturing = 1;
pthread_t thrd1,thrd2;
struct message_queue queue;
Workspace_single_ssp ybs_ssp;
struct objOverall workspace;
FILE* fssp_output;

jmethodID WackupCallback;
jmethodID VadCallback;
jmethodID AudioCallback;

unsigned int capture_sample(FILE *file, unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            enum pcm_format format, unsigned int period_size,
                            unsigned int period_count);

void sigint_handler(int sig __unused)
{
    capturing = 0;
}

int testSSP()
{

    FILE* fp_input;
    FILE* fp_output;
    fp_input = fopen("/sdcard/ssp_input2.pcm", "rb+");
    fp_output = fopen("/sdcard/test_out.pcm", "w");

    Workspace_single_ssp ybs_ssp;
    ybs_ssp = ybs_createEmptyOverallContext();
    struct objOverall workspace;
    workspace = createEmptyOverallContext();
    parameter_int(&ybs_ssp);
    init_fun(&workspace);

    while (!feof(fp_input))
    {

        fread(ybs_ssp.data_buffer->audio_raw_data, sizeof(int16_t), data_accept1 , fp_input);

        //AEC
        ybs_aec_process((ybs_ssp.aec_ssp), (ybs_ssp.data_buffer));

        //SSL SST  BF
        process_fun(&workspace,ybs_ssp.data_buffer->buffer_out ,ybs_ssp.data_buffer->buffer_output );

        //ANS
        ybs_ssp_process(ybs_ssp.data_buffer->buffer_output, ybs_ssp.ans_ssp->nsHandle);

        //VAD
        WebRtcVad_Process(ybs_ssp.vad_ssp->vadInst, 16000, ybs_ssp.data_buffer->buffer_output, 160, 0, &(ybs_ssp.vad_ssp->flag), &(ybs_ssp.vad_ssp->u), &(ybs_ssp.vad_ssp->v), &(ybs_ssp.vad_ssp->vad_smooth_lowtop), &(ybs_ssp.vad_ssp->vad_smooth_toplow));

        //audio output
        output_fun(ybs_ssp.vad_ssp->flag, ybs_ssp.vad_ssp->cache, ybs_ssp.data_buffer->buffer_output, ybs_ssp.data_buffer->buffer_final_out, (ybs_ssp.data_buffer->buffer_cache));

        vadflag = ybs_ssp.vad_ssp->flag ;

        if (vadflag == 1)
            fwrite(ybs_ssp.data_buffer->buffer_final_out, 1, 320, fp_output);


    }

    function_free(&ybs_ssp , &workspace);
    fclose(fp_input);
    fclose(fp_output);
    getchar();
    return 0;
}

int runAlsa()
{
    FILE *file;
    struct wav_header header;
    unsigned int card = 0;
    unsigned int device = 0;
    unsigned int channels = 8;
    unsigned int rate = 16000;
    unsigned int bits = 16;
    unsigned int frames;
    unsigned int period_size = 1024;
    unsigned int period_count = 4;
    enum pcm_format format;


    file = fopen("/sdcard/test.pcm", "wb");
    if (!file) {
        LOGD( "Unable to create file \n");
        return 1;
    }

    header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = channels;
    header.sample_rate = rate;

    switch (bits) {
        case 32:
            format = PCM_FORMAT_S32_LE;
            break;
        case 24:
            format = PCM_FORMAT_S24_LE;
            break;
        case 16:
            format = PCM_FORMAT_S16_LE;
            break;
        default:
            LOGD( "%d bits is not supported.\n", bits);
            return 1;
    }

    header.bits_per_sample = pcm_format_to_bits(format);
    header.byte_rate = (header.bits_per_sample / 8) * channels * rate;
    header.block_align = channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;

    /* leave enough room for header */
    fseek(file, sizeof(struct wav_header), SEEK_SET);

    /* install signal handler and begin capturing */
    signal(SIGINT, sigint_handler);
    frames = capture_sample(file, card, device, header.num_channels,
                            header.sample_rate, format,
                            period_size, period_count);
    LOGD("Captured %d frames\n", frames);

    /* write header now all information is known */
    header.data_sz = frames * header.block_align;
    header.riff_sz = header.data_sz + sizeof(header) - 8;
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(struct wav_header), 1, file);

    fclose(file);

    return 0;
}

unsigned int capture_sample(FILE *file, unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            enum pcm_format format, unsigned int period_size,
                            unsigned int period_count)
{
    ybs_ssp = ybs_createEmptyOverallContext();
    workspace = createEmptyOverallContext();
    parameter_int(&ybs_ssp);
    init_fun(&workspace);

    struct pcm_config config;
    struct pcm *pcm;
    char *buffer;
    unsigned int size;
    unsigned int bytes_read = 0;

    memset(&config, 0, sizeof(config));
    config.channels = channels;
    config.rate = rate;
    config.period_size = period_size;
    config.period_count = period_count;
    config.format = format;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    pcm = pcm_open(card, device, PCM_IN, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        LOGD( "Unable to open PCM device (%s)\n",
              pcm_get_error(pcm));
        return 0;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = malloc(size);
    if (!buffer) {
        LOGD( "Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm);
        return 0;
    }

    LOGD("Capturing sample: %u ch, %u hz, %u bit\n", channels, rate,
         pcm_format_to_bits(format));

    while (capturing && !pcm_read(pcm, buffer, size)) {
        LOGD("size = %d\n",size);
        struct audiomsg *message = message_queue_message_alloc_blocking(&queue);
        message->size = size;
        memcpy(message->buf,buffer,size);
        message_queue_write(&queue, message);
//        if (fwrite(buffer, 1, size, file) != size) {
//            LOGD("Error capturing sample\n");
//            break;
//        }
//        bytes_read += size;
    }

    function_free(&ybs_ssp , &workspace);
    free(buffer);
    pcm_close(pcm);
    return pcm_bytes_to_frames(pcm, bytes_read);
}


void handleAudioSSP(JNIEnv *env, jobject obj){
    struct audiomsg *message;
    int len = 2048;
    uint16_t audiobuf[len/2];
    uint8_t  audiobuf_byte[len];
    int count = 0;
    while(message = message_queue_tryread(&queue)) {
        LOGD("read data size %d\n",message->size);
        int allpacksize = message->size/len;
        for(int packindex = 0;packindex < allpacksize;packindex ++){
            memcpy(audiobuf_byte,message->buf+packindex*len,len);
            for(int index = 0;index < len/2;index ++){
                audiobuf[index] = (uint16_t)(audiobuf_byte[index*2+1] << 8) | audiobuf_byte[index*2];
            }
            //char audioBuf[512] = {1,2,3,4,5};
            jbyteArray data =(*env)->NewByteArray(env,len);
            (*env)->SetByteArrayRegion(env,data,0,len, audiobuf_byte);
            //(*env).CallVoidMethod(env,obj, method2,buf , 3);
            (*env)->CallVoidMethod(env,obj, AudioCallback, data,len);
            (*env)->DeleteLocalRef(env, data);

            fwrite(audiobuf, 1, len, fssp_output);
//            memcpy(ybs_ssp.data_buffer->audio_raw_data,audiobuf,len/2);
//            //AEC
//            ybs_aec_process((ybs_ssp.aec_ssp), (ybs_ssp.data_buffer));
//            //SSL SST  BF
//            process_fun(&workspace,ybs_ssp.data_buffer->buffer_out ,ybs_ssp.data_buffer->buffer_output );
//            //ANS
//            ybs_ssp_process(ybs_ssp.data_buffer->buffer_output, ybs_ssp.ans_ssp->nsHandle);
//            //VAD
//            WebRtcVad_Process(ybs_ssp.vad_ssp->vadInst, 16000, ybs_ssp.data_buffer->buffer_output, 160, 0, &(ybs_ssp.vad_ssp->flag), &(ybs_ssp.vad_ssp->u), &(ybs_ssp.vad_ssp->v), &(ybs_ssp.vad_ssp->vad_smooth_lowtop), &(ybs_ssp.vad_ssp->vad_smooth_toplow));
//            //audio output
//            output_fun(ybs_ssp.vad_ssp->flag, ybs_ssp.vad_ssp->cache, ybs_ssp.data_buffer->buffer_output, ybs_ssp.data_buffer->buffer_final_out, (ybs_ssp.data_buffer->buffer_cache));
//            vadflag = ybs_ssp.vad_ssp->flag ;
//            if (vadflag == 1)
//                fwrite(ybs_ssp.data_buffer->buffer_final_out, 1, 320, fssp_output);
        }

        message_queue_message_free(&queue, message);
    }
}

//录音线程
void * runRecord(void *arg){
    LOGD("runRecord\n");
    runAlsa();
}

//前端处理线程
void * runAudioSSP(void *arg){

}

void cancelGuestRobot(){
    pthread_exit(thrd1);
    pthread_exit(thrd2);
}




void runAudio(JNIEnv *env, jobject obj){

    LOGD("start Audio Read\n");
    jclass clazz = (*env)->FindClass(env,"com/ybs/audiojnilib/AudioModule");
    if(clazz == 0){
        LOGD("find class error");
        return;
    }
    LOGD("find class");
    AudioCallback = (*env)->GetMethodID(env,clazz,"AudioCallback","([BI)V");
    if(AudioCallback == 0){
        LOGD("find AudioCallback error");
        return;
    }
    LOGD("find AudioCallback");
    VadCallback = (*env)->GetMethodID(env,clazz,"VadCallback","(I)V");
    if(VadCallback == 0){
        LOGD("find VadCallback error");
        return;
    }
    LOGD("find VadCallback");
    WackupCallback = (*env)->GetMethodID(env,clazz,"WackupCallback","(II)V");
    if(WackupCallback == 0){
        LOGD("find WackupCallback error");
        return;
    }
    LOGD("find WackupCallback");

    //(*env)->CallVoidMethod(env,obj, AudioCallback, data,len);
    //(*env)->CallVoidMethod(obj, WackupCallback, 3,5);
    //(*env)->CallVoidMethod(obj, VadCallback, 3);

    fssp_output = fopen("/sdcard/test_outssp.pcm", "wb");
    if (!fssp_output) {
        LOGD( "Unable to create fssp_output file \n");
        return ;
    }

    message_queue_init(&queue, sizeof(struct audiomsg), 128);
    int ret;
    ret = pthread_create(&thrd1, NULL,runRecord, NULL);
    ret = pthread_create(&thrd2, NULL,runAudioSSP, NULL);
    for(;;){
        handleAudioSSP(env,obj);
        usleep(10);
    }
    pthread_join(thrd1, NULL);
    pthread_join(thrd2, NULL);
}

