/*

    waitpid 함수의 이해..
    첫번쨰 인자는 -1이면 임의의 자식 프로세스.
    두번째 인자는, 여러가지 정보가 합해진 듯 하다.
    하위 8비트는 자식 프로세스가 리턴한 값을 가지고 있고,
    상위 비트에는 뭐가 있을까?
    세번째 인자 WNOHANG이면 자식 프로세스가 끝나지 않았을때 wait처럼 계속 블럭되지 않고 반환값으로 0을 리턴함.

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, int argv[]){
    int status = 0;
    pid_t pid = fork();

    if(pid == 0){
        sleep(5);
        return 24;
    }
    else{
        printf("Child pid : %d\n" , pid);

        while(!waitpid(-1,&status,WNOHANG)){
            sleep(1);
            puts("sleep 1 sec.");            
        }
        printf("status : %d \n",status);
        if(WIFEXITED(status))
        printf("Child send %d \n", WEXITSTATUS(status)); 
    }
}