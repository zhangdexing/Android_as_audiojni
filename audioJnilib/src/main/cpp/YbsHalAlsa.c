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
#include <jni.h>
#include "unistd.h"
#include <android/log.h>
#include <strings.h>
#include <stdbool.h>
#include "YbsHalAlsa.h"
#include "YbsHalHid.h"
//#include "YbsFrameQueue.h"

//#include "ybs_mutl_ssp-master/dsplib/ybs_mutl_ssp.h"
//#include "ybs_mutl_ssp-master/dsplib/overallContext.h"

int vadflag = 0;



static const char *LOG_TAG="ALSA";

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

jmethodID WackupCallback;
jmethodID VadCallback;
jmethodID AudioCallback;
jmethodID ModuleStatusCallback;


const  uint8_t  READ_STATUS = 1;
const  uint8_t  WAKE_UP = 1;
const  uint8_t VAD_STATE_START = 2;
const  uint8_t VAD_STATE_END = 3;
const  uint8_t VAD_STATE_TIMEOUT = 4;

char buff[8];
bool flag = false;
uint8_t mstatus;
uint carnum;

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

//
unsigned int capture_sample(FILE *file, unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            enum pcm_format format, unsigned int period_size,
                            unsigned int period_count);


unsigned int capture_sample(FILE *file, unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            enum pcm_format format, unsigned int period_size,
                            unsigned int period_count)
{
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
        LOGE("=============== Unable to open PCM device : (%s)",pcm_get_error(pcm));
        return 0;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = malloc(size);
    if (!buffer) {
        LOGE("===============Unable to allocate  byte \n");
        free(buffer);
        pcm_close(pcm);
        return 0;
    }

    printf("Capturing sample: %u ch, %u hz, %u bit\n", channels, rate,
           pcm_format_to_bits(format));
    LOGD("=============== capture_sampleing\n");


    while ( !pcm_read(pcm, buffer, size) ) {
//        LOGD("=============== size=%d\n",size);
        if (fwrite(buffer, 1, size, file) != size) {
            fprintf(stderr,"Error capturing sample\n");
            break;
        }
        bytes_read += size;
        if (mstatus == VAD_STATE_END || mstatus == VAD_STATE_TIMEOUT){//将vadend或vadtimeout后的frame捕获
            for (int i = 0; i < 5 ; ++i) {
                pcm_read(pcm, buffer, size);
                if (fwrite(buffer, 1, size, file) != size) {
                    fprintf(stderr,"Error capturing sample\n");
                    break;
                }
            }
            break;
        }

    }
    LOGD("=============== capture_stop\n");
    free(buffer);
    pcm_close(pcm);
    return pcm_bytes_to_frames(pcm, bytes_read);
}

int UacCaptureRecording() {

    FILE *file;
    struct wav_header header;

    unsigned int card = carnum;
    unsigned int device = 0;
    unsigned int channels = 1;
    unsigned int rate = 16000;
    unsigned int bits = 16;
    unsigned int frames;
    unsigned int period_size = 1024;
    unsigned int period_count = 4;
    enum pcm_format format = PCM_FORMAT_S16_LE;

    header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = channels;
    header.sample_rate = rate;
//
    header.bits_per_sample = pcm_format_to_bits(format);
    header.byte_rate = (header.bits_per_sample / 8) * channels * rate;
    header.block_align = channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;




    while (1) {

        if (mstatus == WAKE_UP || mstatus == VAD_STATE_START) {//begin to captrue
            file = fopen("/data/test.wav", "wb");
            if (!file) {
                LOGE("Unable to create file '/data/test.wav'");
                return -1;
            }
            /* leave enough room for header */
            fseek(file, sizeof(struct wav_header), SEEK_SET);

            frames = capture_sample(file, card, device, channels,
                                    rate, format,
                                    period_size, period_count);
            LOGD("Captured %d frames\n", frames);

            /* write header now all information is known */
            header.data_sz = frames * header.block_align;
            header.riff_sz = header.data_sz + sizeof(header) - 8;
            fseek(file, 0, SEEK_SET);
            fwrite(&header, sizeof(struct wav_header), 1, file);
            fclose(file);
        } else
            usleep(100);

    }
    LOGE("===============Captured %d frames\n", frames);

}
void *StartUacRecording(void *arg)
{
    LOGD("==========UacCaptureRecording");
    /*开始采集音频pcm数据*/
    while (1) {
        if (flag == true)
            UacCaptureRecording();
        else
            usleep(500);
    }
}


void handleAudioSSP(JNIEnv *env, jobject obj) {

    carnum = checkhiddev();
    (*env)->CallVoidMethod(env, obj, ModuleStatusCallback,5);//ok
    if(carnum != -1)//find a hid  device!
    {
        if (inithiddev() < 0) {//open hid dev
            LOGE(LOG_TAG, " open hid dev error;");
            flag = false;
        } else
            flag = true;

        (*env)->CallVoidMethod(env, obj, ModuleStatusCallback,6);//ok
        while (flag == true) {
            bzero(buff, 8);
            if (readtohid(buff, 8) < 0) {
                LOGE(LOG_TAG, " read hid dev error;");
                break;
            }
            if (buff[2] == READ_STATUS)//read vadstatus
            {
                mstatus = buff[3];
                if (buff[3] == WAKE_UP) {
                    (*env)->CallVoidMethod(env, obj, WackupCallback, WAKE_UP, buff[4], buff[5]);
                } else
                    (*env)->CallVoidMethod(env, obj, VadCallback, buff[3]);
            }
        }
    }

}


void runtask(JNIEnv *env, jobject obj){

        LOGD("start Audio Read");
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
        WackupCallback = (*env)->GetMethodID(env,clazz,"WackupCallback","(III)V");
        if(WackupCallback == 0){
            LOGD("find WackupCallback error");
            return;
        }
        LOGD("find WackupCallback");


        ModuleStatusCallback = (*env)->GetMethodID(env,clazz,"ModuleStatusCallback","(I)V");
        if(WackupCallback == 0){
            LOGD("find ModuleStatusCallback error");
            return;
        }
        LOGD("find ModuleStatusCallback");

        pthread_t tid;
        pthread_create(&tid, NULL, StartUacRecording, NULL);//捕获Android端PCM数据
        LOGD("enter pthread~~");
        for(;;){
            handleAudioSSP(env,obj);//always to find hid dev
            sleep(3);
            LOGD("=====7=====No Find a Hid device");
        }
}

