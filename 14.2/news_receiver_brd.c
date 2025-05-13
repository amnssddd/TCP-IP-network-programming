#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int recv_sock;
    struct sockaddr_in adr;
    int str_len;
    char buf[BUF_SIZE];
    struct timeval tv;
    if(argc!=2)
    {
        printf("Usage: %s <port> \n", argv[0]);
        exit(1);
    }

    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[1]));

    if(bind(recv_sock, (struct sockaddr*)&adr, sizeof(adr)) == -1)
        error_handling("bind() error!");

    tv.tv_sec = 5;  // 5秒超时
    tv.tv_usec = 0;
    //如果在指定时间内没有收到数据，recvfrom将返回-1并设置errno为EAGAIN或EWOULDBLOCK
    setsockopt(recv_sock, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(tv));

    while (1) {
        str_len = recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);
        if(str_len < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Receive timeout, exit normally\n");
                break;
            } else {
                perror("recvfrom");
            }
        }
        buf[str_len] = 0;
        fputs(buf, stdout);
    }
    close(recv_sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}