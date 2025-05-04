#include <stdio.h>
#include <unistd.h>
#include <signal.h>

//定义信号处理函数，这类函数称为信号处理器（Handler）
void timeout(int sig)
{
    if(sig == SIGALRM)
        puts("alarm out!");
    alarm(2); //为了每隔2s重复产生SIGALRM信号，在信号处理器调用alarm函数
}

//同上
void keycontrol(int sig)
{
    if(sig == SIGINT)
        puts("CTRL+C pressed");
}

int main()
{
    int i;
    //注册SIGALRM、SIGINT信号及相应处理器
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);

    alarm(2);  //预约2s后发生SIGALRM信号

    for(i=0; i<3; i++)
    {
        puts("wait...");
        //为查看信号产生和信号处理器的执行并提供每次100s、共3次的等待时间（实际执行时运行时间没有10s）
        sleep(100);  //实际上2s后被SIGALRM信号打断进入timeout函数，输出 “alarm out”，进入下一次循环
    }
    return 0;
}