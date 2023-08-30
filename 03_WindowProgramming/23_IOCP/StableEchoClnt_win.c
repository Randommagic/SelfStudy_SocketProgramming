#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024

void ErrorHandling(char *message);

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAdr;
    char message[BUF_SIZE];
    int strLen, readLen;

    if (argc != 3) // 실행파일의 경로/IP/PORT번호를 인자로 받아야 함
    {
        printf("Usage: %s <IP> <port> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // 윈속 라이브버리 초기화
        ErrorHandling("WSAStartup() error");

    /* 소켓은 처음 생성되면 블로킹 모드이므로 넌-블로킹 모드로 변환 */
    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("socket() error");

    /* 서버 주소정보 초기화 */
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    /* 서버 주소정보를 기반으로 연결요청 */
    if (connect(hSocket, (SOCKADDR *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {
        printf("code : %d\n", WSAGetLastError());
        ErrorHandling("connect() error");
    }

    else
        puts("Connected...");

    while (1) {
        /* 클라이언트가 서버로 전송할 문자열 입력 */
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        /* 클라이언트 종료 조건 */
        if (!strcmp(message, "Q\n") || !strcmp(message, "q\n"))
            break;

        strLen = strlen(message);
        send(hSocket, message, strLen, 0);

        readLen = 0;
        /*
        TCP는 데이터의 경계가 없으므로,
        전송한 데이터를 서버로부터 다시 전부 수신할 때까지 반복
        */
        while (1) {
            readLen += recv(hSocket, &message[readLen], BUF_SIZE - 1, 0);
            if (readLen >= strLen)
                break;
        }
        message[strLen] = '\0';
        printf("Message from server: %s", message);
    }

    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}