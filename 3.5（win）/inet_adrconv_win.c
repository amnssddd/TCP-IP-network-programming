#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    //inet_addr函数调用示例
    {
        char *addr = "127.212.124.78";
        unsigned long conv_addr = inet_addr(addr);
        if(conv_addr == INADDR_NONE)
            printf("Error occured! \n");
        else 
            printf("Network ordered interger addr: %#lx \n", conv_addr);
    }

    //inet_ntoa函数调用示例
    {
        struct sockaddr_in addr;
        char *strPtr;
        char strArr[20];

        addr.sin_addr.s_addr = htonl(0x1020304);
        strPtr = inet_ntoa(addr.sin_addr);
        strcpy(strArr, strPtr);
        printf("Dotted-Decimal notation3 %s \n", strArr);
    }

    WSACleanup();
    return 0;
}

void ErrorHandling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}