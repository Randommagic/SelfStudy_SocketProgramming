/*
    getsockopt의 사용
    SO_TYPE 과 SO_SNDBUF

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]) {
    
    int tcp_sock, udp_sock;
    int sock_type;
    socklen_t optlen;
    int state;

    optlen = sizeof(sock_type);
    tcp_sock = socket(PF_INET,SOCK_STREAM,0);
    udp_sock = socket(PF_INET,SOCK_DGRAM,0);
    printf("SOCK_STREAM: %d \n", SOCK_STREAM);
    printf("SOCK_DGAM: %d \n", SOCK_DGRAM);
    // SOL_SOCKET / SO_TYPE
    //tcp 확인
    state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE,(void*)&sock_type, &optlen);
    if(state)
        ErrorHandling("getsocketopt() error!");
    printf("Socket type one : %d \n", sock_type);

    //udp 확인
    state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE,(void*)&sock_type, &optlen);
    if(state)
        ErrorHandling("getsocketopt() error!");
    printf("Socket type one : %d \n", sock_type);

    //SOL_SOCKET / SO_SNDBUF, SO_RCVBUF
    int snd_buf, rcv_buf;

    optlen = sizeof(snd_buf);
    state = getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*) &snd_buf, &optlen);
    if(state)
        ErrorHandling("getsocketopt() error!");


    optlen = sizeof(rcv_buf);
    state = getsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (void*) &rcv_buf, &optlen);
    if(state)
        ErrorHandling("getsocketopt() error!");

    printf("Input Buffer Size : %d \n", rcv_buf);
    printf("Output Buffer Size : %d \n", snd_buf);

    // set으로 SO_SNDBUF 크기만 변경시켜보기
    rcv_buf = 1024 * 300;
    
    state = setsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*) &rcv_buf, sizeof(rcv_buf));
    if(state)
        ErrorHandling("getsocketopt() error!");

    optlen = sizeof(rcv_buf) ;
    state = getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*) &rcv_buf, &optlen);
    if(state)
        ErrorHandling("getsocketopt() error!");
    printf("Input Buffer Size After Change: %d \n", rcv_buf);

    return 0;
}