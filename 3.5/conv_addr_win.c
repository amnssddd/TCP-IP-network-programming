#undef UNICODE
#undef _UNICODE
#include <stdio.h>
#include <winsock2.h>

int main(int argc,char* argv[])
{
    char *strAddr = "203.211.218.102:9190";

    char strAddrBuf[50];
    SOCKADDR_IN servAddr;
    int sizeStrToAddr;          // 用于 WSAStringToAddress
    DWORD sizeAddrToStr;        // 用于 WSAAddressToString

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    sizeStrToAddr = sizeof(servAddr);
    WSAStringToAddress(strAddr, AF_INET, NULL, (SOCKADDR*)&servAddr, &sizeStrToAddr);

    sizeAddrToStr = sizeof(strAddrBuf);
    WSAAddressToString((SOCKADDR*)&servAddr, sizeof(servAddr), NULL, strAddrBuf, &sizeAddrToStr);

    printf("Second conv result: %s \n", strAddrBuf);
    WSACleanup();
    return 0;
}