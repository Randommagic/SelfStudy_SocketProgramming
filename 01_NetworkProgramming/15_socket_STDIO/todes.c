/*
    todes.c
    fileno 함수의 사용 예제
    인지로 file 포인터를 전달하면,  해당 파일의 파일 디스크립터가 반환됨.
*/

#include <stdio.h>
#include <fcntl.h>

int main(void){
    FILE *fp;
    int fd = open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);
    if(fd == -1){
        fputs("file open error", stdout);
        return -1;
    }

    printf("First file descriptor : %d \n" , fd);
    fp = fdopen(fd,"w");

    if(fp == NULL){
        fputs("fp error", stdout);
        return -1;
    }

    fputs("TCP/IP socket programming \n", fp);
    printf("Second file descriptor : %d \n" , fileno(fp));
    fclose(fp);
    return 0;
}