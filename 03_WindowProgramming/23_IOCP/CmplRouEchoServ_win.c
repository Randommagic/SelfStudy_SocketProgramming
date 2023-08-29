/*
    CmplRouEchoServ_win.c
    Complete Routine을 사용한 에코 서버 예제.
    IOCP 사용 X
*/

#include <stdio.h>
#include <stdlib.h>
#include <winerror.h>
#include <winsock2.h>

#define BUF_SIZE 1024

void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(char *msg);

typedef struct {
    SOCKET hClntSock;
    char buf[BUF_SIZE];
    WSABUF wsaBuf;
} PRE_IO_DATA, *LPPER_IO_DATA;

int main(int argc, char *argv[]) {
    if (argc != 2)
        exit(1);

    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAdr, recvAdr;
    LPWSAOVERLAPPED lpOvLp;
    DWORD recvBytes;
    LPPER_IO_DATA hbInfo;
    int recvAdrSz;
    u_long mode = 1;
    DWORD flagInfo = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup Error!");

    // WSASocket 설정
    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hLisnSock == INVALID_SOCKET)
        ErrorHandling("socket creation error");
    ioctlsocket(hLisnSock, FIONBIO, &mode); // 리슨소켓을 non-block으로 만듬

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

    while (1) {
        // 100 ms마다 aceept 호출
        SleepEx(100, TRUE);
        // 위에서 ioctlsocket으로 non-block으로 만들었으므로, 바로 리턴
        hRecvSock = accept(hLisnSock, (SOCKADDR *)&recvAdr, &recvAdrSz);
        // connect한 client가 없을때는, INVALID_SOCKET 반환.
        if (hRecvSock == INVALID_SOCKET) {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
                continue;
            else
                ErrorHandling("accept error");
        }
        puts("Client connected");

        // 클라이언트 한개당 OVERLAPPED 구조체 한개를 새로 만들어야 하기에, 동적 할당.
        lpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
        memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));

        // 클라이언트 소켓정보, 메세지 buf 등을 담을 구조체 동적 할당
        hbInfo = (LPPER_IO_DATA)malloc(sizeof(LPPER_IO_DATA));
        hbInfo->hClntSock = (DWORD)hRecvSock;
        (hbInfo->wsaBuf).buf = hbInfo->buf;
        (hbInfo->wsaBuf).len = BUF_SIZE;

        // Completion routine을 사용하기에, Event는 필요가 없다.
        // 그래서 그 대신 우리가 사용할 PER_IO_DATA를 넣어서, lpoverlapped구조차게 routine함수 매개변수로 넘어가게 함.
        lpOvLp->hEvent = (HANDLE)hbInfo;

        // lpOvLp 는 ReadCompRoutine의 세번째 인자(lpoverlapped)로 들어감.
        // 1. 클라이언트가 접속하면 호출되고, non-block으로 데이터 수신된 후 수신 완료시 ReadCompRoutine 호출.
        WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOvLp, ReadCompRoutine);
    }

    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

void CALLBACK ReadCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD sentBytes;

    if (szRecvBytes == 0) {
        closesocket(hSock);
        free(lpOverlapped->hEvent);
        free(lpOverlapped);
        puts("Client Disconnected");
    } else {
        bufInfo->len = szRecvBytes;
        /// 2. non-block으로 데이터가 송신되게 하고, 송신이 완료되면 WriteCompRoutine 호출.
        WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, WriteCompRoutine);
    }
}

void CALLBACK WriteCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD recvBytes, flagInfo = 0;
    // 3. 다시 WSARecv를 호출하면서, non-blocking 모드로 데이터 수신을 기다림.
    WSARecv(hSock, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped, ReadCompRoutine);
}

void ErrorHandling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}