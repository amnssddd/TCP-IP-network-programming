#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#define BUF_SIZE 1024
void error_handling(char* message);

//文件传输函数
void request_file(int sock, char* filename);
void receive_file(int sock, char* filename);

int main(int argc, char* argv[])
{
    int sock;
    char filename[BUF_SIZE];
    struct sockaddr_in serv_adr;

    if(argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error!");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");
    else
        puts("Connected..................");

    //获取用户输入的文件名
    fputs("Enter filename to request: ", stdout);
    fgets(filename, BUF_SIZE, stdin);
    filename[strlen(filename)-1] = '\0';  //去除换行符

    //请求并接收文件
    request_file(sock, filename);
    receive_file(sock, filename);

    close(sock);
    return 0;
}

//请求文件函数
void request_file(int sock, char* filename)
{
    //发送文件名到服务器
    write(sock, filename, strlen(filename));
    printf("Requesting file: %s\n", filename);
}

//接收文件函数
void receive_file(int sock, char* filename)
{
    int fd;
    char buf[BUF_SIZE];
    ssize_t bytes_received;

    //创建文件
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1)
        error_handling("File creation error!");

    //接收文件内容
    while ((bytes_received = read(sock, buf, BUF_SIZE)) > 0)
    {
        if(write(fd, buf, bytes_received) == -1)
            error_handling("File write error!");
    }
    
    if(bytes_received == -1)
        error_handling("read() error!");

    // 在文件末尾添加换行符
    //write(fd, "\n", 1);

    printf("File sent successfully: %s\n", filename);

    //重新以只读方式打开文件
    fd = open(filename, O_RDONLY);
    if(fd == -1)
        error_handling("File open for reading error!");

    printf("\nReceived file content:\n");
    printf("---------------------------\n");
    while ((bytes_received = read(fd, buf, BUF_SIZE)) > 0)
    {
        write(STDOUT_FILENO, buf, bytes_received);  //直接输出到标准输出
    }
    printf("\n---------------------------\n");
    
    close(fd);
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}