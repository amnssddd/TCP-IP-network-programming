#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;

    FD_ZERO(&reads);    //初始化fd_set变量
    FD_SET(0, &reads);  //将文件描述符0对应的位设置为1

    while (1)
    {
        //调用select函数后，除发生变化的文件描述符对应位外，剩下的所有位将初始化0。因此为记住初始值必须经过该复制过程
        temps = reads;

        //每次调用select函数前都会初始化新值。调用select函数后结构成员的值会被替换为超时前剩余时间，所以需在循环内初始化
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        //调用select函数。如果有控制台输入数据，则返回大于0的整数；如果没有输入数据而引发超时，则返回0
        result = select(1, &temps, 0, 0, &timeout);
        if(result == -1)
        {
            puts("select() error");
            break;
        }
        else if(result == 0)
        {
            puts("Time-out!");
        }
        else
        {
            //验证发生变化的文件描述符是否为标准输入。若是，则从标准输入读取数据并向控制台输出
            if(FD_ISSET(0, &temps))
            {
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }
    return 0;
}