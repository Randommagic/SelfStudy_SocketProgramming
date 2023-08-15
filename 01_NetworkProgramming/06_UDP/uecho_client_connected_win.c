/*
    uecho_client_win.c
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
    SOCKET hSocket;
    char message[BUF_SIZE];
    int strLen;
    SOCKADDR_IN servAdr, fromAdr;

    if (argc != 3) {
        exit(-1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("wsastartuperror");
    }
    // SOCK_STREAM 대신 SOCK_DGRAM
    hSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("socket creation error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    // connected UDP
    connect(hSocket, (SOCKADDR *)&servAdr, sizeof(servAdr));

    while (1) {
        fputs("Input message ( Q to quit ) : ", stdout);
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        send(hSocket, message, strlen(message), 0);
        strLen = recv(hSocket, message, sizeof(message) - 1, 0);
        message[strLen] = 0;
        printf("Message from server : %s", message);
    }

    closesocket(hSocket);
    WSACleanup();
    return 0;
}