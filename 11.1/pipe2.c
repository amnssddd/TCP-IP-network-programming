#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int arc, char* argv[])
{
    int fds[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message!";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds);
    pid = fork();
    if(pid == 0)
    {
        write(fds[1], str1, sizeof(str1));  //向父进程传输数据
        sleep(2);  //不能省略，否则会运行错误，具体原因后面会说
        read(fds[0], buf, BUF_SIZE);        //接收父进程传输的数据
        printf("Child proc output: %s \n", buf);
    }  
    else
    {
        read(fds[0], buf, BUF_SIZE);        //接收子进程传输的数据
        printf("Parents proc output: %s \n", buf);
        write(fds[1], str2, sizeof(str2));  //向子进程传输数据
        sleep(3);
    }
    return 0;
}