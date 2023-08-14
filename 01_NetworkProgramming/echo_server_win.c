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
    SOCKET hServSock, hClntSock;
    char message[BUF_SIZE];
    int strLen, i;

    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSize;

    if (argc != 2) {
        printf("add port\n");
        return -1;
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("wsastartuperror");
    }

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket creation error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (SOCKADDR *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("socket binding error");

    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen error");

    clntAdrSize = sizeof(clntAdr);

    for (i = 0; i < 5; i++) {
        hClntSock = accept(hServSock, (SOCKADDR *)&clntAdr, &clntAdrSize);
        if (hClntSock == INVALID_SOCKET)
            ErrorHandling("accept socket error");
        else
            printf("Connected client %d\n", i);

        while ((strLen = recv(hClntSock, message, BUF_SIZE, 0)) != 0)
            send(hClntSock, message, strLen, 0);
        closesocket(hClntSock);
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}