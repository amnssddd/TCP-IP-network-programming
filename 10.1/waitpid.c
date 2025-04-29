#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    int status;
    pid_t pid = fork();

    if(pid == 0)
    {
        sleep(15);  //推迟子进程的执行
        return 24;
    }
    else
    {
        //while函数调用waitpid函数。向第三个参数传递 WNOHANG，因此若没有终止的子进程将返回0
        while (!waitpid(pid, &status, WNOHANG))
        {
            sleep(3);
            puts("sleep 3sec.");
        }
        
        if(WIFEXITED(status))
            printf("Child send %d \n", WEXITSTATUS(status));
    }
    return 0;
}