#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#define BUF_SIZE 1024
void error_handling(char* message);

int main(int argc, char* argv[])
{
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;
    struct ip_mreq join_adr;
    struct timeval tv;
    if(argc!=3)
    {
        printf("Usage: %s <IP> <port> \n", argv[0]);
        exit(1);
    }

    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[2]));

    if(bind(recv_sock, (struct sockaddr*)&adr, sizeof(adr)) == -1)
        error_handling("bind() error");

    //初始化结构体ip_merg
    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);  //初始化多播组地址
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);   //初始化待加入主机的IP地址
    //利用套接字选项IP_ADD_MEMBERSHIP加入多播组。至此完成了接收多播组数据的所有准备
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_adr, sizeof(join_adr));

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