/*
    pipe 사용 예제
    두개의 파이프를 만들어서, 부모와 자식 프로세스 사이의 통신 예제.
*/

#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char * argv[]){

    int fds1[2], fds2[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds1);
    pipe(fds2);

    pid = fork();

    if(pid == 0){
        write(fds1[1],str1,sizeof(str1));
        read(fds2[0],buf,BUF_SIZE);
        printf("child proc output : %s \n" , buf);
    }else{
        read(fds1[0],buf, BUF_SIZE);
        printf("parent proc output : %s \n" , buf);
        write(fds2[1],str2,sizeof(str2));
        sleep(3);        
    }
}