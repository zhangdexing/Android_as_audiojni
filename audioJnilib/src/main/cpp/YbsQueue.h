//
// Created by dexing on 2021/5/19.
//

#ifndef MY_APPLICATION_YBSQUEUE_H
#define MY_APPLICATION_YBSQUEUE_H

#endif //MY_APPLICATION_YBSQUEUE_H

//对wav文件的分析和封装


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include<stdbool.h>




#define ARR_SIZE 500
#define DATA_SIZE  256
struct info //数据对象可修改
{
    //char data[DATA_SIZE];//存储数据
    char *data;//存储数据

};

struct seq_queue
{
    struct info   arr[ARR_SIZE];
    // char out_buf[DATA_SIZE];//出队数组
    char *out_buf;//出队数组
    int front;//队头
    int rear;//队尾
    int  datalen;
    int count;
};
// struct seq_queue  queue;

void init_queue(struct seq_queue  *queue,pthread_mutex_t *mutex,int buffsize);
bool  is_queue_empty(struct seq_queue  *queue);

bool is_queue_full(struct seq_queue  *queue);
int in_queue(struct seq_queue  *queue,pthread_mutex_t *mutex,const char *buf);
int out_queue(struct seq_queue  *queue,pthread_mutex_t *mutex);
int atom_out_queue(struct seq_queue  *queue);







#ifdef __cplusplus
} /* extern "C" */
#endif /* C++ */
