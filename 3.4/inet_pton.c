#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
    char *addr = "172.232.124.79";
    struct sockaddr_in addr_inet;

    if(inet_pton(AF_INET, addr, &addr_inet.sin_addr) <= 0)
        error_handling("Conversion error");
    else
        printf("Network ordered interger addr: %#x \n", addr_inet.sin_addr.s_addr);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
