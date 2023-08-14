/*
    hello_client_win과 비슷하지만, 1바이트씩 계속해서 read를 통해 데이터를 받아온다.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
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
    SOCKADDR_IN servAdr;

    if (argc != 3) {
        exit(-1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("wsastartuperror");
    }

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
        printf("Connected..\n");

    while (1) {
        fputs("Input message ( Q to quit ) : ", stdout);
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        send(hSocket, message, strlen(message), 0);
        strLen = recv(hSocket, message, BUF_SIZE - 1, 0);
        message[strLen] = 0;
        printf("Message from server : %s", message);
    }

    closesocket(hSocket);
    WSACleanup();
    return 0;
}