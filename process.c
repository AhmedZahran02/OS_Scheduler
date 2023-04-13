
#include "headers.h"

/* Modify this file as needed*/

int remainingtime;
Process *shmCurrProcess;

int main(int agrc, char *argv[])
{
    initClk();

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    int shm_Id = shmget(CONNKEY + 1, 40, 0666 | IPC_CREAT);
    shmCurrProcess = (Process *)shmat(shm_Id, (void *)0, 0);
    remainingtime = shmCurrProcess->remRunTime;

    shmCurrProcess->startingTime = getClk();
    // printf("process id %d start %d reman %d remtime %d realid %d \n", shmCurrProcess->id, shmCurrProcess->startingTime, shmCurrProcess->runTime, shmCurrProcess->remRunTime, shmCurrProcess->realID);
    // if (shmCurrProcess == -1)
    // {
    //     perror("Error in attach in writer");
    //     exit(-1);
    // }
    while (remainingtime > 0)
    {
        sleep(1);
        remainingtime--;
        shmCurrProcess->remRunTime = remainingtime;
        //    printf("%d \n", shmCurrProcess->remRunTime);
    }

    // shmCurrProcess->finishTime = getClk();
    // printf(" process %d terminated \n", shmCurrProcess->id);

    // destroyClk(false);

    return 0;
}
