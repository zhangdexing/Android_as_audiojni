//C file for SystemInformation
//YBSzhang
//2020.12.17
#define  _GNU_SOURCE
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <jni.h>
#include <fcntl.h>
#include <unistd.h>
#include <android/log.h>
#include "errno.h"

#include "dirent.h"



#define  TAG    "YbsHid"
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)


int fd=0;
//#define  DEV_PATH "/dev/hidraw0"


char * FindHidName(){
    char *path = "/dev/";
    //printf("the target path:%s\n",path);
    DIR *dp = opendir(path);
    struct dirent *eq;
    char *name[15];
    char buff[20];
    bzero(buff,20);
    //char *output[2]={".",".."};
    int i,j=0;
    for ( i = 1; (eq = readdir(dp)) !=NULL ;  ++i)
    {
        char *p1 = strstr(eq->d_name,"hidraw");//用strstr（查找字符串）判断格式是否正确；
        //char *p2 = strstr(eq->d_name,"hello");
        if(p1 != NULL )
        {
            name[j]=eq->d_name;
            //printf("[%d]%s\n",j+1,name[j]);
            snprintf(buff,20,"/dev/%s",name[j]);
            j++;
            return buff;
        }
    }
    return NULL;
}

 char * checkhiddev(){
    char *path = "/dev/";
    //printf("the target path:%s\n",path);
    DIR *dp = opendir(path);
    struct dirent *eq;
    char buff[20];
    char *name[15];
    bzero(buff,20);
    //char *output[2]={".",".."};
    int i,j=0;
    for ( i = 1; (eq = readdir(dp)) !=NULL ;  ++i)
    {
        char *p1 = strstr(eq->d_name,"hidraw");//用strstr（查找字符串）判断格式是否正确；
        //char *p2 = strstr(eq->d_name,"hello");
        if(p1 != NULL )
        {
            name[j]=eq->d_name;
            //printf("[%d]%s\n",j+1,name[j]);
            snprintf(buff,20,"/dev/%s",name[j]);
            j++;
            return buff;
        }
    }
    return NULL;
}

int inithiddev(){
    char *HidName = FindHidName();
    //LOGI("jni========HidName=%s\n",HidName);
    if (HidName == NULL)
    {
        LOGI("Can't find Hid Devices \n");
        return -1;
    }

    fd = open(HidName, O_RDWR);
    if(fd < 0){
        LOGI("open error errno = %d \n",errno);
        return  -1;
    }
    LOGI("========open HID suc\n");
    //return 0;
    return  0;
}

//描述：read。
char * readtohid() {
    int len = 8;
    const char buflen = 0x08;
    char buf[8];
    //LOGI("====开始读取HID信息.\n");

    bzero(buf, buflen);
    int n = read(fd, buf, buflen);

    if(n < 0 ){
        LOGI("====read faile.\n");
        return  NULL;//return  data
    }
    if(buf[0] != 0x08)
    {
        LOGI("====data faile.\n");
    }

    return  buf;//return  data
}


//描述：write。
 int  writetohid(int type , int value) {
    char buf[8];
    bzero(buf,8);
    const char buflen = 0x08;
    buf[0] = 0x08;//head
    buf[1] = 0x01;//datalen
    buf[2] = type;//1：status,  2：angle ，3：volume
    buf[3] = value;
    int ret;
    //LOGI("[write to hid]----> buf[3]=%d \n",value);
    ret = write(fd, buf, buflen);
    if(ret < 0)
    {
        LOGI("[write ]---->error  ret=%d \n",ret);
        return -1;
    }

    //LOGI("[write to hid]---->return  \n");
    return 0;
}

#define HID_STATUS 1

#define HID_WAKEUP 1
#define HID_VAD_START 2
#define HID_VAD_END 3
#define HID_VAD_TIMEOUT 4

int runYbsHid(){
    system("su -c \"chmod 777 /dev/hidraw0\"");
    if(NULL == checkhiddev()){
        LOGD("Can find Hid Device\n");
        return -1;
    }
    for(;;){
        char *hiddata = readtohid();
        if(hiddata[2] == HID_STATUS){
            switch (hiddata[3]) {
                case HID_WAKEUP:
                    LOGD("HID_WAKEUP\n");
                    break;
                case HID_VAD_START:
                    LOGD("HID_VAD_START\n");
                    break;
                case HID_VAD_END:
                    LOGD("HID_VAD_END\n");
                    break;
                case HID_VAD_TIMEOUT:
                    LOGD("HID_VAD_TIMEOUT\n");
                    break;
            }
        }
        usleep(1000);
    }
}