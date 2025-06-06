#include <stdio.h>
#include <pthread.h>
void * thread_sumation(void* arg);
int sum = 0;

int main()
{
    pthread_t id_t1, id_t2;
    int range1[] = {1, 5};
    int range2[] = {6, 10};

    pthread_create(&id_t1, NULL, thread_sumation, (void*)range1);
    pthread_create(&id_t2, NULL, thread_sumation, (void*)range2);

    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);

    printf("result: %d \n", sum);
    return 0;
}

void * thread_sumation(void* arg)
{
    int start = ((int*)arg)[0];
    int end = ((int*)arg)[1];

    while (start <= end)
    {
        sum+=start;
        start++;
    }
    return NULL;
}
