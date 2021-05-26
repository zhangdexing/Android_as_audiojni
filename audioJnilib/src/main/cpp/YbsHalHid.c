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
#include <pthread.h>
#include <stdbool.h>
#include "errno.h"

#include "dirent.h"
#include "YbsHalHid.h"

#define  TAG    "YbsHid"
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

int fd=0;
char hidname[20];

//char * FindHidName(){
//    char *path = "/dev/";
//    //printf("the target path:%s\n",path);
//    DIR *dp = opendir(path);
//    struct dirent *eq;
//    char *name[15];
//    char buff[20];
//    bzero(buff,20);
//    //char *output[2]={".",".."};
//    int i,j=0;
//    for ( i = 1; (eq = readdir(dp)) !=NULL ;  ++i)
//    {
//        char *p1 = strstr(eq->d_name,"hidraw");//用strstr（查找字符串）判断格式是否正确；
//        //char *p2 = strstr(eq->d_name,"hello");
//        if(p1 != NULL )
//        {
//            name[j]=eq->d_name;
//            //printf("[%d]%s\n",j+1,name[j]);
//            snprintf(buff,20,"/dev/%s",name[j]);
//            j++;
//            return buff;
//        }
//    }
//    return NULL;
//}

int checkhiddev(){
    char *path = "/dev/";
    //printf("the target path:%s\n",path);
    DIR *dp = opendir(path);
    struct dirent *eq;
    char buff[100];
    char *name[15];
    bzero(buff,100);
    //char *output[2]={".",".."};
    int i,j=0;
    for ( i = 1; (eq = readdir(dp)) !=NULL ;  ++i)
    {
        char *p1 = strstr(eq->d_name,"hidraw");//用strstr（查找字符串）判断格式是否正确；
        //char *p2 = strstr(eq->d_name,"hello");
        if(p1 != NULL )
        {
            name[j]=eq->d_name;
            snprintf(hidname,20,"/dev/%s",name[j]);//save hidname

            snprintf(buff,100,"su -c \"chmod 777 /dev/%s\"",name[j]);//chomd 777 hidname
            system(buff);
            bzero(buff,100);

            system("su -c \"chmod 777 /proc/asound/cards\"");//chomd 777 /proc/asound/cards
            int cardnum = findcardnumber();//find uac cardnumber
            if (cardnum < 0)
            {
                LOGE("findcardnumber failed.\n");
                return -1;
            }

            snprintf(buff,100,"su -c \"chmod 777 /dev/snd/pcmC%dD0c\"",cardnum);//chomd 777 /dev/snd/pcm
            system(buff);
            return cardnum;
        }
    }
    return -1;
}
int findcardnumber(){
    int fd = open("/proc/asound/cards",O_RDONLY);
    if (fd < 0){
        LOGI("open /proc/asound/cards  failed\n");
        return -1;
    }
    char buff[5000];
    int i =0;
    while(1)
    {
        read(fd,buff+i,1);
        if (fd < 0){
            LOGI("read /proc/asound/cards  failed\n");
            return -1;
        }
        i++;
        if(strstr(buff,"UacDev") != NULL)
        {
            break;
        }
        if(i > 5000)
            break;
    }
    int cardnum;
    char *p = "UacDev";
    char *tmp;
    tmp = strstr(buff,p);
    if(tmp != NULL)
    {
        cardnum = atoi(tmp-3);
        LOGI("cardnum : %d\n",cardnum);
    }
    else {
        LOGI("找不到文本长度\n");
        return -1;
    }
    return cardnum;

}



int openhiddev(){

    fd = open(hidname, O_RDWR);
    if(fd < 0){
        LOGI("open error errno = %d \n",errno);
        return  -1;
    }
    LOGI("========open HID suc\n");
    return  0;
}

//描述：read。
int readtohid(char *buf,int len) {
    bzero(buf, len);
    int n = read(fd, buf, len);
    if(n < 0 ){
        LOGI("====read faile.\n");
        return  -1;
    }
    return  0;
}


//描述：write。
 int  writetohid(int type , int value) {
    char buf[8];
    bzero(buf,8);
    const char buflen = 0x08;
    buf[0] = 0x08;//head
    buf[1] = 0x01;//datalen
    buf[2] = type;//1：status,  2：volume
    buf[3] = value;
    int ret;
    //LOGI("[write to hid]----> buf[3]=%d \n",value);
    ret = write(fd, buf, buflen);
    if(ret < 0)
    {
        LOGI("[write ]---->error  ret=%d \n",ret);
        return -1;
    }

    LOGI("[write to hid]---->return  \n");
    return 0;
}

void setVoid(int vol){
    writetohid(2,vol);
    //system("echo \"hello\" > /dev/hidraw1");
}

int getVol(){
    return 50;
}

int runYbsHid(){
    int ret;

    system("su -c \"chmod 777 /dev/hidraw1\"");
    if(NULL == checkhiddev()){
        LOGD("Can find Hid Device\n");
        return -1;
    }
    LOGD("checkHid dev success\n");
    openhiddev();
    char readbuf[8];
    for(;;){
        if(0 == readtohid(readbuf,sizeof(readbuf))){
            LOGD("hid get data %s\n",readbuf);
        }
        usleep(10000);

    }
}