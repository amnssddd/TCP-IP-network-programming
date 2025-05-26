#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[])
{
    pthread_t t_id;
    int thread_param = 5;

    /* 请求创建一个线程，从thread_main函数调用开始，在单独的执行流中运行。
       同时在调用thread_main函数时向其传递thread_param变量的地址值 */
    if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param) != 0)
    {
        puts("ptread_creat() error!");
        return -1;
    }
    sleep(10);
    puts("ends of main");
    return 0;
}

//该函数参数为pthread_create函数的第四个参数 (void*)&thread_param
void* thread_main(void *arg)
{
    int i;
    int cnt = *((int*)arg);
    for(i=0; i<cnt; i++)
    {
        sleep(1);
        puts("running thread");
    }
    return NULL;
}
