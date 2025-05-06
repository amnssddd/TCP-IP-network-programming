#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char* message);
void handle_sigint(int sig);

int main(int argc, char* argv[])
{
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr;
    struct sigaction sigint_act;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>", argv[0]);
        exit(1);
    }

    //设置SIGINT信号处理
    sigint_act.sa_handler = handle_sigint;
    sigemptyset(&sigint_act.sa_mask);
    sigint_act.sa_flags = 0;
    sigaction(SIGINT, &sigint_act, 0);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    while(1)
    {
        int str_len = read(sock , buf, BUF_SIZE);
        if(str_len == 0)
            break;
        buf[str_len] = 0;
        printf("Message from server: %s \n", buf);
    }

    close(sock);
    return 0;
}

void handle_sigint(int sig)
{
    char answer;
    printf("\nDo you want to quit? [Y/n] ");
    answer = getchar();
    if(answer == 'y' || answer == 'Y')
        exit(1);
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}