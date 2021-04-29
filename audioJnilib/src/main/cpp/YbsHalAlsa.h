
#ifndef _YBS_HAL_ALSA_H_
#define _YBS_HAL_ALSA_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

int runAlsa();
int testSSP();
void runAudio(JNIEnv *env, jobject obj);

#ifdef __cplusplus
}
#endif
#endif
