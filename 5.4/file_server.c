#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#define BUF_SIZE 1024
void error_handling(char* message);

void send_file(int sock, char* filename);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    char filename[BUF_SIZE];
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    
    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error!");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");

    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
    if(clnt_sock == -1)
        error_handling("accept() error!");

    //接收客户端请求的文件名
    ssize_t len = read(clnt_sock, filename, BUF_SIZE-1);  // 留一个字节给'\0'
    if(len <= 0)
        error_handling("read filename error!");
    filename[len] = '\0';  // 手动添加终止符

    printf("Client requested file: %s\n", filename);

    //发送文件
    send_file(clnt_sock, filename);

    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void send_file(int sock, char* filename)
{
    int fd;
    char buf[BUF_SIZE];
    ssize_t bytes_read;

    //尝试打开文件
    fd = open(filename, O_RDONLY);
    if(fd == -1)
    {
        printf("File not found: %s\n", filename);
        close(sock);
        return;
    }

    //读取并发送文件内容
    while((bytes_read = read(fd, buf, BUF_SIZE)) > 0)
    {
        if(write(sock, buf, bytes_read) == -1)
            error_handling("write() error!");
    }

    if(bytes_read == -1)
        error_handling("read() error!");

    printf("File sent successfully: %s\n", filename);
    close(fd);
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
