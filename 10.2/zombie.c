#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    pid_t pid = fork();

    if(pid == 0)
    {
        puts("Hi, I am a child process");
    }
    else
    {
        //可通过子进程ID来查看该进程是否为僵尸进程
        printf("Child Process ID: %d \n", pid);
        sleep(30); //父进程暂停30s，若父进程终止，处于僵尸状态的子进程将同时销毁。因此需延缓父进程的进行
    }

    if(pid == 0)
        puts("End child process");
    else
        puts("End parent process");

    return 0;
}