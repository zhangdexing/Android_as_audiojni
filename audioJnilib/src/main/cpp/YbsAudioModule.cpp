/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <string>
#include <unistd.h>
#include "com_ybs_audiojnilib_AudioModule.h"
#include "android/log.h"

#include <sys/stat.h>
#include "YbsHalAlsa.h"
#include "YbsHalHid.h"

static const char *TAG="serial_port";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)



/* Header for class Java_com_ybs_audiojnilib_AudioModule */

/*
 * Class:     Java_com_ybs_audiojnilib_AudioModule
 * Method:    getVersion
 * Signature: ()Ljava/lang/String;
 */
extern "C"
JNIEXPORT jstring JNICALL Java_com_ybs_audiojnilib_AudioModule_getVersion
        (JNIEnv *env, jobject){
    std::string hello = "V1.0.1";
    return env->NewStringUTF(hello.c_str());
}

/*
 * Class:     Java_com_ybs_audiojnilib_AudioModule
 * Method:    run
 * Signature: ()I
 */
extern "C"
JNIEXPORT jint JNICALL Java_com_ybs_audiojnilib_AudioModule_run
        (JNIEnv *env, jobject obj){
    system("su -c \"chmod 777 /dev/snd/pcmC0D0c\"");
//    runAudio(env,obj);
    return 1;
}


/*
 * Class:     Java_com_ybs_audiojnilib_AudioModule
 * Method:    SetVol
 * Signature: (I)I
 */
extern "C"
JNIEXPORT jint JNICALL Java_com_ybs_audiojnilib_AudioModule_setVol
        (JNIEnv *env, jobject obj, jint vol){
    setVoid(vol);
    return 1;
}

/*
 * Class:     Java_com_ybs_audiojnilib_AudioModule
 * Method:    SetVol
 * Signature: (I)I
 */
extern "C"
JNIEXPORT jint JNICALL Java_com_ybs_audiojnilib_AudioModule_getVol
        (JNIEnv *env, jobject obj){
    return getVol();
}



extern "C"
JNIEXPORT jint JNICALL Java_com_ybs_audiojnilib_AudioModule_runuac
        (JNIEnv *env, jobject obj) {
//    system("su -c \"chmod 777 /dev/snd/pcmC0D0c\"");
    runtask(env, obj);
    return 1;

}
