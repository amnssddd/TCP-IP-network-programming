#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char* message);
void read_childproc(int sig);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int fds[2];

    pid_t pid;
    struct sigaction act;
    char buf[BUF_SIZE];
    socklen_t adr_sz;
    int str_len, state;
    if(argc!=2)
    {
        printf("Usage: %s <port> \n", argv[0]);
        exit(1);
    }

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");

    pipe(fds); //创建管道，fds[0]是读取端，fds[1]是写入端
    pid = fork();
    if(pid == 0)
    {
        FILE * fp = fopen("echomsg.txt", "wt");
        if(fp == NULL) {
            perror("fopen failed");
            exit(1);
        }
        char msgbuf[BUF_SIZE];
        ssize_t len;
        
        close(fds[1]); //关闭不需要的写入端（子进程仅负责接收管道信息）
        
        while((len = read(fds[0], msgbuf, BUF_SIZE)) > 0) {
            /* 采用如下写法会将终端输入一同写入txt文件，产生大量乱码，故用fputc函数过滤非可打印字符
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void*)msgbuf, 1, len, fp);
            */
            
            //过滤掉非可打印字符
            for(int i=0; i<len; i++) {
                //ASCII值<32的字符大多是控制字符，但保留了换行符和对齐符
                if(msgbuf[i] >= 32 || msgbuf[i] == '\n' || msgbuf[i] == '\t') {
                    fputc(msgbuf[i], fp);
                }
            }
            fflush(fp);  //确保数据立即写入文件，而不是留在缓冲区
        }
        fclose(fp);
        close(fds[0]); //传输完所有信息后关闭子进程管道的读取端
        exit(0);
    }
    else //父进程
    {
        close(fds[0]); //关闭不需要的读取端（父进程仅负责传输管道信息）
    }
    while(1)
    {
        adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
        if(clnt_sock == -1)
            continue;
        else
            puts("new client connected...");

        pid = fork();
        if(pid == 0)
        {
            close(serv_sock);
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
            {
                write(fds[1], buf, str_len);
                write(clnt_sock, buf, str_len);
            }
            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        else
            close(clnt_sock);
    }
    close(fds[1]);   //主循环结束后关闭父进程管道写入端，否则子进程会一直等待 EOF，变成僵尸进程，在此子进程会随着客户端的关闭而退出，故没有影响
    close(serv_sock);
    return 0;
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("reomve proc id: %d \n", pid);
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}