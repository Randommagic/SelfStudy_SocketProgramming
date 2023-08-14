/*
    uecho_server_win.c
    간단한 window UDP통신 테스트
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 30

void ErrorHandling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]) {

    WSADATA wsaData;
    SOCKET servSock;
    char message[BUF_SIZE];
    int strLen, clntAdrSize;
    SOCKADDR_IN servAdr, clntAdr;

    if (argc != 2) {
        exit(-1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("wsastartuperror");

    // SOCK_STREAM 대신 SOCK_DGRAM
    servSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (servSock == INVALID_SOCKET)
        ErrorHandling("socket creation error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (SOCKADDR *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    while (1) {
        clntAdrSize = sizeof(clntAdr);
        strLen = recvfrom(servSock, message, BUF_SIZE, 0, (SOCKADDR *)&clntAdr, &clntAdrSize);
        sendto(servSock, message, strLen, 0, (SOCKADDR *)&clntAdr, sizeof(clntAdr));
    }

    closesocket(servSock);
    WSACleanup();
    return 0;
}