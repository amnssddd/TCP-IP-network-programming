#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#define OPSZ 4
int caculate(int opnum, int opnds[], char oprator);
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    char opinfo[BUF_SIZE];
    char opndCntChar;  // 用于接收1字节的操作数个数
    int result, opndCnt, i;
    int recvCnt, recvLen;  //单次 recv 调用实际读取的字节数；累计已接受的字节数
    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSize;

    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if(hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error!");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));
    
    if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error!");

    if(listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error!");

    clntAdrSize = sizeof(clntAdr);
    for(i=0; i<5; i++)
    {
        opndCnt = 0;
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);

        if(recv(hClntSock, &opndCntChar, 1, 0) <= 0) {
            closesocket(hClntSock);
            continue;  
        }
        opndCnt = (int)opndCntChar;  // 转换为int类型

        recvLen = 0;
        while(recvLen < opndCnt*OPSZ+1)
        {
            recvCnt = recv(hClntSock, &opinfo[recvLen], BUF_SIZE-1, 0);
            recvLen += recvCnt;
        }
        result = caculate(opndCnt, (int*)opinfo, opinfo[recvLen-1]);
        send(hClntSock, (char*)&result, sizeof(result) ,0);
        closesocket(hClntSock);
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

int caculate(int opnum, int opnds[], char op)
{
    int result = opnds[0], i;
    switch (op)
    {
    case '+':
        for(i=1; i<opnum; i++) result += opnds[i];
        break;
    case '-':
        for(i=1; i<opnum; i++) result -= opnds[i];
        break;
    case '*':
        for(i=1; i<opnum; i++) result *= opnds[i];
        break;
    }
    return result;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}