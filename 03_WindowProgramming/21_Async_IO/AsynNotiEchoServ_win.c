#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 100
void CompressSockets(SOCKET hSockArr[], int idx, int total);
void CompressEvents(WSAEVENT hEventArr[], int idx, int total);
void ErrorHandling(const char *message);

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;

    SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT newEvent;
    WSANETWORKEVENTS netEvents;

    int numOfClntSock = 0;
    int strLen;
    int posInfo, startIdx;
    int clntAdrLen;
    char msg[BUF_SIZE];

    if (argc != 2) {
        printf("add port\n");
        return -1;
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("wsastartuperror");
    }
    // socket 설정
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

    newEvent = WSACreateEvent();
    // 서버 소켓이 ACCEPT 연결 요청을 받으면 -> newEvent 의 signal을 on.

    if (WSAEventSelect(hServSock, newEvent, FD_ACCEPT) == SOCKET_ERROR)
        ErrorHandling("wsaeventselect error");

    hSockArr[numOfClntSock] = hServSock;
    hEventArr[numOfClntSock] = newEvent;
    numOfClntSock++;

    while (1) {
        posInfo = WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
        startIdx = posInfo - WSA_WAIT_EVENT_0;

        for (int i = startIdx; i < numOfClntSock; i++) {
            int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);
            if (sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT)
                continue;
            else {
                sigEventIdx = i;
                WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);
                // 연결 요청
                if (netEvents.lNetworkEvents & FD_ACCEPT) {
                    if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
                        puts("Accept error");
                        break;
                    }
                    clntAdrLen = sizeof(clntAdr);
                    hClntSock = accept(hSockArr[sigEventIdx], (SOCKADDR *)&clntAdr, &clntAdrLen);
                    newEvent = WSACreateEvent();
                    WSAEventSelect(hClntSock, newEvent, FD_READ | FD_CLOSE);

                    hEventArr[numOfClntSock] = newEvent;
                    hSockArr[numOfClntSock] = hClntSock;
                    numOfClntSock++;
                    puts("Connected new client\n");
                }
                // 데이터 수신
                if (netEvents.lNetworkEvents & FD_READ) {
                    if (netEvents.iErrorCode[FD_READ_BIT] != 0) {
                        puts("READ error");
                        break;
                    }
                    strLen = recv(hSockArr[sigEventIdx], msg, BUF_SIZE, 0);
                    send(hSockArr[sigEventIdx], msg, strLen, 0);
                }
                // 종료 요청
                if (netEvents.lNetworkEvents & FD_CLOSE) {
                    if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0) {
                        puts("CLOSE error");
                        break;
                    }
                    WSACloseEvent(hEventArr[sigEventIdx]);
                    closesocket(hSockArr[sigEventIdx]);

                    numOfClntSock--;
                    CompressSockets(hSockArr, sigEventIdx, numOfClntSock);
                    CompressEvents(hEventArr, sigEventIdx, numOfClntSock);
                }
            }
        }
    }
    WSACleanup();
    return 0;
}

void ErrorHandling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void CompressSockets(SOCKET hSockArr[], int idx, int total) {
    for (int i = idx; i < total; i++)
        hSockArr[i] = hSockArr[i + 1];
}

void CompressEvents(WSAEVENT hEventArr[], int idx, int total) {
    for (int i = idx; i < total; i++)
        hEventArr[i] = hEventArr[i + 1];
}