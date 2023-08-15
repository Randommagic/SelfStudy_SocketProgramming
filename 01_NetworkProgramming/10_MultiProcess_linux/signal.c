/*
    signal.c
    signal 함수에 대한 이해.
    
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
    signal(SIGALRM, timeout);
    signal(SIGINT, keydown);
    alarm(2);

    for(int i = 0 ; i < 30;i++){
        sleep(5);
    }
    return 0;
}