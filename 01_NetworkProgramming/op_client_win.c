/*
    hello_client_win과 비슷하지만, 1바이트씩 계속해서 read를 통해 데이터를 받아온다.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void ErrorHandling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hSocket;
    char message[BUF_SIZE];
    int result, opCnt = 0;
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

    ////
    fputs("Operand Count : ", stdout);
    scanf("%d", &opCnt);
    message[0] = (char)opCnt;

    for (int i = 0; i < opCnt; i++) {
        printf("Operand %d : ", i + 1);
        scanf("%d", (int *)&message[i * OPSZ + 1]);
    }
    fgetc(stdin);
    fputs("Operator : ", stdout);
    scanf("%c", &message[opCnt * OPSZ + 1]);

    send(hSocket, message, opCnt * OPSZ + 2, 0);
    recv(hSocket, (char *)&result, RLT_SIZE, 0);

    printf("Result : %d \n", result);
    ////
    closesocket(hSocket);
    WSACleanup();
    return 0;
}