#include <process.h>
#include <stdio.h>
#include <windows.h>
unsigned WINAPI ThreadFunc(void *arg);

int main(int argc, char *argv[]) {
    HANDLE hThread;
    DWORD wr;
    unsigned threadID;
    int param = 5;
    hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void *)&param, 0, &threadID);
    if (hThread == 0) {
        puts("beginthreadex error");
        return -1;
    }

    if ((wr = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED) {
        puts("Thread wait error");
        return -1;
    }

    puts("end of Main");
    return 0;
}

unsigned WINAPI ThreadFunc(void *arg) {
    int cnt = *((int *)arg);
    for (int i = 0; i < cnt; i++) {
        Sleep(1000);
        puts("running thread");
    }
    return 0;
}