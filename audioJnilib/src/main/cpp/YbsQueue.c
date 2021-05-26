//
// Created by dexing on 2021/5/19.
//

#include <strings.h>
#include "YbsQueue.h"
/*
 * @Copyright (C), 2020 : YBS
 * @内容简述:
 * @version:
 * @Author: zdx
 * @LastEditors: zdx
 * @LastEditTime: 2021-04-23 17:02:12
 */

//---------------------------------------------queue

void init_queue(struct seq_queue  *queue,pthread_mutex_t *mutex,int buffsize)
{
    queue->front=0;
    queue->rear = 0 ;
    queue->count = 0;

    queue->datalen = buffsize;
    // 初始化互斥锁
    if (pthread_mutex_init(mutex, NULL) != 0){
        // 互斥锁初始化失败
        printf("=======互斥锁初始化失败\n");
        return ;
    }


    for(int i =0;i<ARR_SIZE;i++){
        queue->arr[i].data = malloc(buffsize);
    }
    queue->out_buf = malloc(buffsize);
    printf("\n=======初始化队列 ok\n");
}

bool  is_queue_empty(struct seq_queue  *queue)
{
    //printf("is_queue_empty~queue.front=%d  queue.rear=%d \n",queue.front,queue.rear);
    return (queue->front == queue->rear);
}


bool is_queue_full(struct seq_queue  *queue)
{
    //printf("is_queue_full~queue.front=%d  queue.rear=%d \n",queue.front,queue.rear);
    return ((queue->rear+1)%ARR_SIZE == queue->front) ;
}

int in_queue(struct seq_queue  *queue,pthread_mutex_t * mutex,const char *buf)
{
    // 先判断是否满
    if(true == is_queue_full(queue))// 满了
    {
        return -1;
    }

    // 加锁
    if (pthread_mutex_lock(mutex) != 0){
        fprintf(stdout, "lock error!\n");
    }

    bzero(queue->arr[queue->rear].data,queue->datalen);
    memcpy(queue->arr[queue->rear].data, buf,queue->datalen);//

    //未满 就入队

    queue->rear = (queue->rear+1)%ARR_SIZE ;
    //printf("in_queue  queue.front=%d  queue.rear=%d~ \n",queue.front,queue.rear);
    // 解锁
    pthread_mutex_unlock(mutex);

    return 1;
}

int out_queue(struct seq_queue  *queue,pthread_mutex_t  *mutex)
{
    // 加锁
    if (pthread_mutex_lock(mutex) != 0){
        fprintf(stdout, "lock error!\n");
    }
    // 先判断是否空
    if(true == is_queue_empty(queue))// 空了
    {
        // // 解锁
        pthread_mutex_unlock(mutex);
        return -1;
    }
    bzero(queue->out_buf,queue->datalen);
    memcpy(queue->out_buf,queue->arr[ queue->front].data,queue->datalen);//保存出队的数据

    //有数据 就出队
    queue->front = (queue->front+1)%ARR_SIZE ;
    //printf("out_queue  queue.front=%d  queue.rear=%d~ \n",queue.front,queue.rear);
    // 解锁
    pthread_mutex_unlock(mutex);
    return 1;
}

int atom_out_queue(struct seq_queue  *queue)
{

    // 先判断是否空
    if(true == is_queue_empty(queue))// 空了
    {
        return -1;
    }
    bzero(queue->out_buf,queue->datalen);
    memcpy(queue->out_buf,queue->arr[ queue->front].data,queue->datalen);//保存出队的数据

    //有数据 就出队
    queue->front = (queue->front+1)%ARR_SIZE ;
    //printf("out_queue  queue.front=%d  queue.rear=%d~ \n",queue.front,queue.rear);

    return 1;
}
//---------------------------------------------
