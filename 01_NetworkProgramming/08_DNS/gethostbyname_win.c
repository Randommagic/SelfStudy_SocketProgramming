/*
    gethostbyname_win.c
    호스트 이름으로 관련 정보 얻기

    gethostbyname_win.exe www.naver.com

    Official name : www.naver.com.nheos.com
    Aliases 1 : www.naver.com
    Address type : AF_INET
    IP addr 1 : 223.130.195.200
    IP addr 2 : 223.130.200.107
*/

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 30

void ErrorHandling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]) {

    WSADATA wsaData;
    struct hostent *host;
    if (argc != 2) {
        exit(-1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("wsastartuperror");

    // gethostbyname
    host = gethostbyname(argv[1]);
    if (!host)
        ErrorHandling("gethostbyname error");

    printf("Official name : %s \n", host->h_name);
    for (int i = 0; host->h_aliases[i]; i++)
        printf("Aliases %d : %s \n", i + 1, host->h_aliases[i]);
    printf("Address type : %s \n", (host->h_addrtype == AF_INET ? "AF_INET" : "AF_INET6"));

    for (int i = 0; host->h_addr_list[i]; i++)
        printf("IP addr %d : %s \n", i + 1, inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));

    WSACleanup();
    return 0;
}