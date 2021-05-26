

#ifndef _YBS_HAL_HID_H_
#define _YBS_HAL_HID_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif
int readtohid(char *buf,int len);
//char * FindHidName();
int checkhiddev();
int openhiddev();
int  writetohid(int type , int value);
//int excpcmsu(void);
int findcardnumber(void);


int runYbsHid();

void setVoid(int vol);

int getVol();

#ifdef __cplusplus
}
#endif
#endif
