/*
    CmplRouEchoClnt_win.c
    Complete Routine을 사용한 에코 클라이언트 예제.
    IOCP 사용 X
*/

#include <stdio.h>
#include <stdlib.h>
#include <winerror.h>
#include <winsock2.h>

#define BUF_SIZE 1024

void ErrorHandling(char *msg);

int main(int argc, char *argv[]) {
    if (argc != 3)
        exit(1);

    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAdr;
    char message[BUF_SIZE];
    int strLen, readLen;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup Error!");

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("socket creation error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    if (connect(hSocket, (SOCKADDR *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("connect error");
    else
        puts("Connected..");

    while (1) {
        fputs("Input message(q to quit) : ", stdout);
        fgets(message, BUF_SIZE, stdin);
        fflush(stdin);
        if (!strcmp(message, "q\n"))
            break;

        strLen = strlen(message);
        send(hSocket, message, strLen, 0);
        readLen = 0;
        while (1) {
            readLen += recv(hSocket, &message[readLen], BUF_SIZE - 1, 0);
            if (readLen >= strLen)
                break;
        }
        message[strLen] = 0;
        printf("Message from server : %s", message);
    }

    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}