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
    SOCKET servSock, clntSock;
    FILE *fp;
    char buf[BUF_SIZE];
    int readCnt;

    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSize;

    if (argc != 2) {
        exit(-1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("wsastartuperror");

    fp = fopen("file_server_win.c", "rb");
    servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock == INVALID_SOCKET)
        ErrorHandling("socket creation error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (SOCKADDR *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    listen(servSock, 5);
    clntAdrSize = sizeof(clntAdr);
    clntSock = accept(servSock, (SOCKADDR *)&clntAdr, &clntAdrSize);

    while (1) {
        readCnt = fread((void *)buf, 1, BUF_SIZE, fp);
        if (readCnt < BUF_SIZE) {
            send(clntSock, buf, readCnt, 0);
            break;
        }
        send(clntSock, buf, BUF_SIZE, 0);
    }
    // 우아한 연결 종료, SD_SEND : 송신 스트림을 닫겠다.
    shutdown(clntSock, SD_SEND);
    // 수신 스트림은 남아있으므로, 클라이언트에서 마지막 메세지들을 받는다.
    recv(clntSock, (char *)buf, BUF_SIZE, 0);
    printf("Message from client : %s", buf);

    fclose(fp);
    closesocket(clntSock);
    closesocket(servSock);
    WSACleanup();
    return 0;
}