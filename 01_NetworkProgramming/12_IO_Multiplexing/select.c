/*
    select.c
    select함수 호출 예제
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;

    // file descripter set 초기화 한 후
    FD_ZERO(&reads);
    // 0 번 fd, standard input을 1로 바꿈으로서 지켜보는 fd로 등록시킴.
    FD_SET(0,&reads);

    while(1){
        // fd set을 복사해서 복사본을 사용
        temps = reads;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        
        // 만약 여기서 standard input을 사용했다면, 0번 fd는 계속 1로 있었겠지만,
        // 사용을 안했다면 select 이후로 0으로 초기화되어, 지켜보지 않음.
        // 그래서 위에서 복사본 사용.
        result = select(1, &temps,0,0,&timeout);
        if(result == -1){
            puts("select error");
            break;
        }
        else if(result == 0){
            puts("time out");
        }
        else{
            if(FD_ISSET(0,&temps)){
                str_len = read(0,buf,BUF_SIZE);
                buf[str_len] = 0;
                printf("message from console : %s \n", buf);
            }
        }
    }
    return 0;
}