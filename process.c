#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
Process *shmCurrProcess;

int main(int agrc, char *argv[])
{
    initClk();

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    shmCurrProcess = shmat(CONNKEY + 1, (void *)0, 0);
    if (shmAddrCnt == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    while (remainingtime > 0)
    {
        // remainingtime = ??;
    }

    destroyClk(false);

    return 0;
}
