#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

//用于管理接入的客户端套接字的变量和数组。访问这两个变量的代码将构成临界区
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];

pthread_mutex_t mutx;

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;
    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]); 
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if((bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))) == -1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");

    while(1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
    
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;  //每当有新连接时，将相关信息写入变量clnt_cnt和clnt_socks
        pthread_mutex_unlock(&mutx);

        //创建线程向新接入的客户端提供服务。由该线程执行handle_cnt函数
        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        //调用pthread_detach函数从内存中完全销毁已终止的线程
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

//处理单个客户端连接，负责接收客户端消息并处理客户端断开连接
void * handle_clnt(void * arg)
{
    int clnt_sock = *((int*)arg);
    int str_len = 0, i;
    char msg[BUF_SIZE];

    //读取客户端信息并广播给所有客户端
    while((str_len=read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    //客户端断开
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_cnt; i++)  // 遍历查找要删除的客户端
    {
        if(clnt_sock == clnt_socks[i])  // 找到匹配的socket
        {
            while(i++<clnt_cnt-1)  // 从找到位置开始前移元素
                clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;  // 客户端计数减1
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);  // 关闭socket
    return NULL;
}

//该函数负责向所有连接的客户端发送消息
void send_msg(char * msg, int len)  //send to all
{
    int i;
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char * msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}