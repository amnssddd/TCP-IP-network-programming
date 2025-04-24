//该客户端是在6.2客户端的基础上修改，所需的服务器端与6.2中的服务器端相同，结果也一致
//故省略其服务器端和运行，观察该代码的connect函数和新的I/O函数即可
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char* message);

int main(int argc, char* argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr, from_adr;  //不再需要from_adr
    socklen_t adr_sz;   //多余变量

    if(argc != 3)
    {
        printf("Usage %s <IP> <port>", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handling("socket() error!");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    //新增connect函数
    connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

    while(1)
    {
        fputs("Insert message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if(!strcmp(message, "q\n") | !strcmp(message, "Q\n"))
            break;
        /*
        sendto(sock, message, sizeof(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        */
        write(sock, message, sizeof(message));

        /*
        adr_sz = sizeof(from_adr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_adr, &adr_sz);
        */
        str_len = read(sock, message, sizeof(message) - 1);

        message[str_len] = 0;  //添加字符串终止符 '\0'
        printf("Message from server: %s", message);
    }

    close(sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc("\n", stderr);
    exit(1);
}