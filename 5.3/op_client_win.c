#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSOCKET;
    char opmsg[BUF_SIZE];
    int result, opndCnt, i;
    SOCKADDR_IN servAdr;
    if(argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hSOCKET = socket(PF_INET, SOCK_STREAM, 0);
    if(hSOCKET == INVALID_SOCKET)
        ErrorHandling("socket() error!");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));
    if(connect(hSOCKET, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");
    else
        puts("Connected...........................");

    fputs("Operand count: ", stdout);
    scanf("%d", &opndCnt);      //输入操作数个数
    opmsg[0] = (char)opndCnt;   //转换为字符型后存入opmsg[0]处，操作数从opmsg[1]开始

    for(i=0; i<opndCnt; i++)
    {
        printf("Operand %d: ", i+1);
        scanf("%d", (int*)&opmsg[i*OPSZ+1]);  //数组退化为char*，将char*型转换为int*型（需要取址）
    }
    fgetc(stdin);
    fputs("Operator: ", stdout);
    scanf("%c", &opmsg[opndCnt*OPSZ+1]); //在数组末存放操作符
    send(hSOCKET, opmsg, opndCnt*OPSZ+2, 0);

    //接收数据
    char resultBuf[RLT_SIZE];  // 使用 char 数组接收数据
    recv(hSOCKET, resultBuf, RLT_SIZE, 0);
    
    // 将字节数据转换为 int，并处理字节序
    result = *(int*)resultBuf;  // 直接解析

    printf("Operation result: %d \n", result);
    closesocket(hSOCKET);
    WSACleanup();
    return 0;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}