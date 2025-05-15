#include <stdio.h>
#include <fcntl.h>

int main(void)
{
    FILE * fp;
    int fd = open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);  //使用open函数创建文件并返回文件描述符
    if(fd == -1)
    {
        fputs("file open error", stdout);
        return -1;
    }

    //调用fdopen函数将文件描述符转换为FILE指针。此时向第二个参数传递了“w”，因此返回写模式的FILE指针
    fp = fdopen(fd, "w");
    fputs("Network C programming \n", fp);
    fclose(fp);  //利用FILE指针关闭文件
    return 0;
}