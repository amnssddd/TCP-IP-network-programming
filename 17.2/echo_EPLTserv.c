#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 4   //将调用read函数时使用的缓冲大小缩减为4个字节
#define EPOLL_SIZE 50
void error_handling(char *buf);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t adr_sz;
    int str_len, i;
    char buf[BUF_SIZE];

    struct epoll_event *epoll_events;
    struct epoll_event event;
    int epfd, event_cnt;

    if(argc!=2){
        printf("Usage: %s <port> \n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");

    epfd = epoll_create(EPOLL_SIZE);  //创建一个 epoll 实例，返回一个文件描述符（后续操作都需要epfd）
    epoll_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    //向 epoll 实例注册、修改或删除文件描述符的监听事件
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while(1)
    {
        //等待 epoll 实例上的 I/O 事件，返回就绪的事件数量
        event_cnt = epoll_wait(epfd, epoll_events, EPOLL_SIZE, -1);
        if(event_cnt == -1)
        {
            puts("epoll_wait() error!");
            break;
        }

        puts("return epoll_wait");  //插入验证epoll_wait调用次数的语句
        for(i=0; i<event_cnt; i++)
        {
            if(epoll_events[i].data.fd == serv_sock)  //新连接处理
            {
                adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                event.events = EPOLLIN;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connect client: %d \n", clnt_sock);
            }
            else  //客户端数据读写处理
            {
                str_len = read(epoll_events[i].data.fd, buf, BUF_SIZE);
                if(str_len == 0)  //close request!
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL);
                    close(epoll_events[i].data.fd);
                    printf("close client: %d \n", epoll_events[i].data.fd);
                }
                else
                {
                    write(epoll_events[i].data.fd, buf, str_len);  //echo!
                    write(epoll_events[i].data.fd, "\nEOF\n", 5); // 添加结束标记
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}