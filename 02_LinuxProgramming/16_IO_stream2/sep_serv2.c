/*
    ser_serv2.c
    송신과 수신을 위한 2개의 FILE POINTER 생성 후,
    각각  fp에 dup된 다른 fd를 연결하고
    half-close 실습하는 예제

*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
void error_handling(char *message);
int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    FILE *readfp;
    FILE *writefp;

    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    char buf[BUF_SIZE] = {
        0,
    };

    if (argc != 2) {
        exit(1);
    }

    // socket설정
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
    if (clnt_sock == -1)
        error_handling("accept error");

    // fd를 복사해서 FILE POINTER 생성
    readfp = fdopen(clnt_sock, "r");
    writefp = fdopen(dup(clnt_sock), "w");

    fputs("FROM SEVER : 1111 \n", writefp);
    fputs("FROM SEVER : 2222 \n", writefp);
    fputs("FROM SEVER : 3333 \n", writefp);
    fflush(writefp);

    shutdown(fileno(writefp), SHUT_WR);
    fclose(writefp);

    fgets(buf, sizeof(buf), readfp);
    fputs(buf, stdout);
    fclose(readfp);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}