/*
    chat_serv_win
    윈도우에서 멀티쓰레드를 사용한 채팅서버
*/

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

unsigned WINAPI HandleClient(void *arg);
void sendMsg(char *msg, int len);
void ErrorHandling(char *msg);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSize;
    HANDLE hThread;

    if (argc != 2) {
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup Error!");

    // Socket 설정
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

    hMutex = CreateMutex(NULL, FALSE, NULL);

    while (1) {
        clntAdrSize = sizeof(clntAdr);
        hClntSock = accept(hServSock, (SOCKADDR *)&clntAdr, &clntAdrSize);

        WaitForSingleObject(&hMutex, INFINITE);
        clntSocks[clntCnt++] = hClntSock;
        ReleaseMutex(&hMutex);

        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, (void *)&hClntSock, 0, NULL);
        printf("Connected Client IP : %s \n", inet_ntoa(clntAdr.sin_addr));
    }

    closesocket(hServSock);
    WSACleanup();
    return 0;
}

unsigned WINAPI HandleClient(void *arg) {
    SOCKET hClntSock = *((SOCKET *)arg);
    char msg[BUF_SIZE];
    int strLen = 0;
    while ((strLen = recv(hClntSock, msg, BUF_SIZE, 0)) != 0)
        sendMsg(msg, strLen);

    WaitForSingleObject(&hMutex, INFINITE);
    for (int i = 0; i < clntCnt; i++) {
        if (hClntSock == clntSocks[i]) {
            while (i++ < clntCnt - 1)
                clntSocks[i] = clntSocks[i + 1];
            break;
        }
    }
    clntCnt--;
    ReleaseMutex(&hMutex);
    closesocket(hClntSock);
    return 0;
}
void sendMsg(char *msg, int len) {
    WaitForSingleObject(&hMutex, INFINITE);
    for (int i = 0; i < clntCnt; i++)
        send(clntSocks[i], msg, len, 0);
    ReleaseMutex(&hMutex);
}
void ErrorHandling(const char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}