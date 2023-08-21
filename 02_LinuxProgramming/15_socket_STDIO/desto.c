
/*

표준 입출력 fputs등을 사용하려면,
파일 디스크립터터를 FILE 포인터로 변환해야 한다.

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

    fp = fdopen(fd, "w");
    fputs("Network C programming \n", fp);
    fclose(fp);
    return 0;


}