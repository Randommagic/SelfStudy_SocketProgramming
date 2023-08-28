/*
    OverlappedSend_win.c
    Overlapped IO client
*/

#include <stdio.h>
#include <stdlib.h>
#include <winerror.h>
#include <winsock2.h>

void ErrorHandling(char *msg);

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN sendAdr;

    if (argc != 3) {
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup Error!");

    WSABUF databuf;
    char msg[] = "Network is Computer!";
    DWORD sendBytes = 0;
    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;

    // WSASocket 설정
    hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("socket creation error");

    // connect할 주소 설정
    memset(&sendAdr, 0, sizeof(sendAdr));
    sendAdr.sin_family = AF_INET;
    sendAdr.sin_addr.s_addr = inet_addr(argv[1]);
    sendAdr.sin_port = htons(atoi(argv[2]));

    if (connect(hSocket, (SOCKADDR *)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR) {
        int errorcode = WSAGetLastError();
        printf("errorcode : %d\n", errorcode);
        ErrorHandling("Connect error");
    }

    evObj = WSACreateEvent();
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = evObj;
    databuf.len = strlen(msg) + 1;
    databuf.buf = msg;

    if (WSASend(hSocket, &databuf, 1, &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() == WSA_IO_PENDING) {
            puts("Background data send");
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
            WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);
        } else
            ErrorHandling("WSASEND error");
    }

    printf("Send data size : %d \n", sendBytes);
    WSACloseEvent(evObj);
    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}