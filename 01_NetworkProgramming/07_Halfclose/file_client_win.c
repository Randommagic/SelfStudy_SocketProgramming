/*
    file_server_win.c
    tcp half-close 예제
    소켓의 우아한 연결 종료.
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
    FILE *fp;
    char buf[BUF_SIZE];
    int readCnt;

    SOCKADDR_IN servAdr;

    if (argc != 3) {
        exit(-1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("wsastartuperror");

    fp = fopen("receive.dat", "wb");
    servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock == INVALID_SOCKET)
        ErrorHandling("socket creation error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    connect(servSock, (SOCKADDR *)&servAdr, sizeof(servAdr));

    while ((readCnt = recv(servSock, buf, BUF_SIZE, 0)) != 0) {
        fwrite((void *)buf, 1, readCnt, fp);
    }

    puts("Received file data");
    send(servSock, "Thank you", 10, 0);

    fclose(fp);
    closesocket(servSock);
    WSACleanup();
    return 0;
}