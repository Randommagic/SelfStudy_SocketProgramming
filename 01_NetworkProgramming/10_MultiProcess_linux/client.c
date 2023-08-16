#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char* message) ;

int main(int argc, char* argv[]){

    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr;

    if (argc != 3) {
        exit(-1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket creation error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect error");
    else
        printf("Connected..\n");

    while (1) {
        fputs("Input message ( Q to quit ) : ", stdout);
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        write(serv_sock, message, strlen(message));
        str_len = read(serv_sock, message, BUF_SIZE - 1);
        message[str_len] = 0;
        printf("Message from server : %s", message);
    }

    close(serv_sock);
    return 0;
}

void error_handling(char* message){
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}