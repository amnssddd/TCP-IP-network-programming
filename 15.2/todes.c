#include <stdio.h>
#include <fcntl.h>

int main(void)
{
    FILE *fp;
    int fd = open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);
    if(fd == -1)
    {
        fputs("flie open error", stdout);
        return -1;
    }

    printf("First file descriptor: %d \n", fd);  //输出文件描述符整数值
    fp = fdopen(fd, "w");  //调用fdopen函数将文件描述符转换为FILE指针
    fputs("TCP/IP SOCKET PROGAMMING \n", fp);
    printf("Second file descriptor: %d \n", fileno(fp));  //调用fileno函数再次转回文件描述符
    fclose(fp);
    return 0;
}