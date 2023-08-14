/*
230814
클라이언트로부터 3개의숫자와 1개의 연산자를 받아서, 결과값 리턴
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

int calculate(int opnum, int opnds[], char op) {
    int result = opnds[0], i;
    switch (op) {
    case '+':
        for (i = 1; i < opnum; i++)
            result += opnds[i];
        break;
    case '-':
        for (i = 1; i < opnum; i++)
            result -= opnds[i];
        break;
    case '*':
        for (i = 1; i < opnum; i++)
            result *= opnds[i];
        break;
    }
    return result;
}

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    char opInfo[BUF_SIZE];
    int opCnt, result;
    int recvCnt, recvLen;

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

    opCnt = 0;

    hClntSock = accept(hServSock, (SOCKADDR *)&clntAdr, &clntAdrSize);
    if (hClntSock == INVALID_SOCKET)
        ErrorHandling("accept socket error");
    else
        printf("Connected client \n");

    recv(hClntSock, (char *)&opCnt, 1, 0);
    //
    recvLen = 0;
    while ((opCnt * OPSZ + 1) > recvLen) {
        recvCnt = recv(hClntSock, opInfo, BUF_SIZE - 1, 0);
        recvLen += recvCnt;
    }
    result = calculate(opCnt, (int *)opInfo, opInfo[recvLen - 1]);

    send(hClntSock, (char *)&result, sizeof(result), 0);
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();
    return 0;
}