/*
    OverlappedSend_win.c
    Overlapped IO server
*/

#include <stdio.h>
#include <stdlib.h>
#include <winerror.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *msg);

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAdr, recvAdr;
    int recvAdrSz;

    if (argc != 2) {
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup Error!");

    char buf[BUF_SIZE];
    DWORD recvBytes = 0, flags = 0;
    WSABUF databuf;
    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;

    // WSASocket 설정
    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hLisnSock == INVALID_SOCKET)
        ErrorHandling("socket creation error");

    // connect할 주소 설정
    memset(&lisnAdr, 0, sizeof(lisnAdr));
    lisnAdr.sin_family = AF_INET;
    lisnAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    lisnAdr.sin_port = htons(atoi(argv[1]));

    if (bind(hLisnSock, (SOCKADDR *)&lisnAdr, sizeof(lisnAdr)) == SOCKET_ERROR)
        ErrorHandling("bind error");

    if (listen(hLisnSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen error");

    recvAdrSz = sizeof(recvAdr);
    hRecvSock = accept(hLisnSock, (SOCKADDR *)&recvAdr, &recvAdrSz);

    // WSA 이벤트 만들기
    evObj = WSACreateEvent();

    // WSAOVERLAPPED 구조체 초기화 후 hevent에 wsa이벤트 등록
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = evObj;

    // WSARecv 2번째 인자로 들어가는 보낼 데이터 정보 초기화.
    databuf.len = BUF_SIZE;
    databuf.buf = buf;

    // WSARecv가 SOCKET_ERROR를 반환하지 않으면, 데이터가 수신 완료된것이고, recvbyte가 의미를 갖는다.

    if (WSARecv(hRecvSock, &databuf, 1, &recvBytes, &flags, &overlapped, NULL) == SOCKET_ERROR) {
        // WSARecv가 socket_error를 반환하고, 그 에러코드가 WSA_IO_PENDING이면, 현재 데이터를 수신중임.
        if (WSAGetLastError() == WSA_IO_PENDING) {
            puts("Background data receive");
            // overlapp이 끝나면 overlapped구조체에 연결된 WSAEvent가 non-signaled -> signaled되어 밑에 줄 통과
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
            WSAGetOverlappedResult(hRecvSock, &overlapped, &recvBytes, FALSE, NULL);

        } else
            ErrorHandling("WSASEND error");
    }

    printf("Send data size : %d \n", recvBytes);
    WSACloseEvent(evObj);
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}