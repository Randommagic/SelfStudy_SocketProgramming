/*
    echo_epollETserv.c

    epoll의 Edge Trigger 방식을 확인하기 위한 예제.
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
//
#include <errno.h>
#include <fcntl.h>

#define BUF_SIZE 4
#define EPOLL_SIZE 50

void set_nonblocking_mode(int);
void error_handling(char *message);

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    int str_len, i;
    char buf[BUF_SIZE];

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

    // epoll 설정
    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd, event_cnt;

    epfd = epoll_create(EPOLL_SIZE);
    ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    // serv_sock 을 Non-blocking으로 설정
    set_nonblocking_mode(serv_sock);

    // serv_sock fd를 epoll에 등록
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    // echo serv start
    while (1) {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1) {
            puts("epoll_wait error");
            break;
        }
        puts("return epoll_wait");
        for (i = 0; i < event_cnt; i++) {
            if (ep_events[i].data.fd == serv_sock) {
                clnt_adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
                if (clnt_sock == -1)
                    error_handling("accept error");

                // epoll에 클라이언트 소켓 등록
                set_nonblocking_mode(clnt_sock);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client : %d \n", clnt_sock);
            } else {
                while (1) {
                    str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                    if (str_len < 0 && errno == EAGAIN) {
                        break;
                    } else if (str_len == 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                        close(ep_events[i].data.fd);
                        printf("disconnected client : %d \n", ep_events[i].data.fd);
                        break;
                    } else {
                        write(ep_events[i].data.fd, buf, str_len);
                    }
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    free(ep_events);
    return 0;
}

void set_nonblocking_mode(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFD, flag | O_NONBLOCK);
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}