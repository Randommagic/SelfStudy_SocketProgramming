/*
    sigaction.c
    sigaction 함수에 대한 이해.
    
*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){
    if(sig == SIGALRM)
        puts("time out!");
    alarm(2);
}

void keydown(int sig){
    if(sig == SIGINT)
        puts("ctrl + c pressed!");
}

int main(int argc, char *argv[]){

    // sigaction 함수 인자로 필요한 sigaction 구조체 초기화
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    
    struct sigaction act2;
    act2.sa_handler = keydown;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = 0;
    


    // signal생성
    sigaction(SIGALRM, &act , 0);
    sigaction(SIGINT, &act2 , 0);


    alarm(2);
    for(int i = 0 ; i < 10;i++){
        puts("sleepin'");
        sleep(5);
    }
    return 0;
}