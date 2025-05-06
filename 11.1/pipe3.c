#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    int fds1[2], fds2[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds1), pipe(fds2); //创建2个管道
    pid = fork();
    if(pid == 0)  //子进程通过数组fds1指向的管道向父进程传输数据
    {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0],  buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    else          //父进程通过数组fds2指向的管道向子进程发送数据
    {
        read(fds1[0], buf, BUF_SIZE);
        printf("Parents proc output: %s \n", buf);
        write(fds2[1], str2, sizeof(str2));
        sleep(3);  //并未有太大意义，仅是为了延迟父进程终止
    }
    return 0;
}