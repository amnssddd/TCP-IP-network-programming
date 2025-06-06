#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100

void * thread_inc(void* arg);
void * thread_des(void* arg);
long long num = 0;
pthread_mutex_t mutex;  //声明保存互斥量读取的变量（全局变量）

int main(int argc, char* argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;

    pthread_mutex_init(&mutex, NULL);

    printf("sizeof long long: %ld \n", sizeof(long long)); //查看long long的大小
    for(i=0; i<NUM_THREAD; i++)
    {
        if(i%2)
            pthread_create(&thread_id[i], NULL, thread_inc, NULL);
        else
            pthread_create(&thread_id[i], NULL, thread_des, NULL);
    }
    for(i=0; i<NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);
    
    printf("result: %lld \n", num);
    pthread_mutex_destroy(&mutex);  //销毁互斥量
    return 0;
}

void * thread_inc(void* arg)
{
    int i;
    pthread_mutex_lock(&mutex);
    for(i=0; i<50000000; i++)
        num+=1;
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void * thread_des(void* arg)
{
    int i;
    pthread_mutex_lock(&mutex);
    for(i=0; i<50000000; i++)
        num-=1;
    pthread_mutex_unlock(&mutex);
    return NULL;
}