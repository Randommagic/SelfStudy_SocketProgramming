/*
    CmplRoutinesRecv_win.c
    Overlapped에 Completion Routine 사용
*/

#include <stdio.h>
#include <stdlib.h>
#include <winerror.h>
#include <winsock2.h>

#define BUF_SIZE 1024

void CALLBACK CompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(char *msg);

WSABUF databuf;
char buf[BUF_SIZE];
DWORD recvBytes = 0;

int main(int argc, char *argv[]) {
    if (argc != 2)
        exit(1);

    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAdr, recvAdr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup Error!");

    DWORD flags = 0;
    int recvAdrSz;
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

    if (hRecvSock == INVALID_SOCKET)
        ErrorHandling("accept error");

    // WSAOVERLAPPED 구조체 초기화 후 hevent에 wsa이벤트 등록
    memset(&overlapped, 0, sizeof(overlapped));
    // overlapped.hEvent = evObj;
    evObj = WSACreateEvent(); // 쓰이지는 않음.
    databuf.len = BUF_SIZE;
    databuf.buf = buf;

    // 마지막 매개변수로 CompRoutine 콜백함수 등록
    if (WSARecv(hRecvSock, &databuf, 1, &recvBytes, &flags, &overlapped, CompRoutine) == SOCKET_ERROR) {
        // WSARecv가 socket_error를 반환하고, 그 에러코드가 WSA_IO_PENDING이면, 현재 데이터를 수신중임.
        if (WSAGetLastError() == WSA_IO_PENDING)
            puts("Background data receive");
    }

    int idx = WSAWaitForMultipleEvents(1, &evObj, FALSE, WSA_INFINITE, TRUE);

    if (idx == WAIT_IO_COMPLETION)
        puts("Overlapped IO completed");
    else
        ErrorHandling("WSARecv error");

    WSACloseEvent(evObj);
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

// 오류정보, 입출력 데이터의 크기정보, wsasend /wsarecv 매개변수 lpoverlapped, 특성정보
void CALLBACK CompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    if (dwError != 0)
        ErrorHandling("CompRoutine error");
    else {
        recvBytes = szRecvBytes;
        printf("Recieved mesage : %s \n", buf);
    }
}

void ErrorHandling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}