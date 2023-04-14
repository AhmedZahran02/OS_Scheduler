
#include "headers.h"

/* Modify this file as needed*/

int remainingtime;
Process *shmCurrProcess;
void stop(int signum);
void cont(int signum);
int currTime;

int main(int agrc, char *argv[])
{
    initClk();
    // signal(SIGUSR1, stop);
    // signal(SIGCONT, cont);

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    int shm_Id = shmget(CONNKEY + 1, 40, 0666 | IPC_CREAT);
    shmCurrProcess = (Process *)shmat(shm_Id, (void *)0, 0);
    remainingtime = shmCurrProcess->remRunTime;

    shmCurrProcess->startingTime = getClk();

    while (remainingtime > 0)
    {
        // currTime = getClk();
        // while (getClk() == currTime)
        //     ;
        // currTime = getClk();
        sleep(1);
        --remainingtime;
        shmCurrProcess->remRunTime = remainingtime;
    }

    return 0;
}

void stop(int signum)
{
    // TODO Clears all resources in case of interruption
    if (remainingtime != shmCurrProcess->remRunTime)
    {
        shmCurrProcess->remRunTime = remainingtime;
    }
    raise(SIGSTOP);
}

void cont(int signum)
{
    // TODO Clears all resources in case of interruption
    currTime = getClk();
    raise(SIGCONT);
}