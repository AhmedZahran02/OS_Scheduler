#include "headers.h"

/* Modify this file as needed*/

int remainingtime;
Process *shmCurrProcess;
int msg_Id = -1;

bool kill_me();
bool initializeMsgQueue();

void nextSecondWaiting(int *lastSecond)
{
    while (*lastSecond == getClk()) ;
    *lastSecond = getClk();
}
int main(int agrc, char *argv[])
{
    initClk();
    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    int shm_Id = shmget(CONNKEY + 1, 40, 0666 | IPC_CREAT);
    shmCurrProcess = (Process *)shmat(shm_Id, (void *)0, 0);
    remainingtime = shmCurrProcess->remRunTime;
    int last = -1;
    shmCurrProcess->startingTime = getClk();

    while (remainingtime > 0)
    {
//        printf("process with pid %d ran for 1 quanta , r = %d \n", getpid(), remainingtime - 1);
        sleep(1);
        remainingtime--;
//        last =getClk();
//        while (last==getClk());
//            remainingtime -= 1;
//            last=getClk();
        shmCurrProcess->remRunTime = remainingtime;
    }
    initializeMsgQueue();
    kill_me();
//    printf("loool process send signal to finsish with id = %d , realId = %d \n", shmCurrProcess->id, shmCurrProcess->realID);
    kill(getppid(), SIGUSR2);
    return 0;
}

bool initializeMsgQueue()
{
    if ((msg_Id = msgget(CLRPKEY, 0666 | IPC_CREAT)) == -1)
    {
        printf("failled to initialize msg queue");
        return false;
    }
    return true;
}

bool kill_me()
{
    struct message *msg = malloc(sizeof(struct message));
    msg->pid = getpid();
    if (msg_Id == -1)
    {
        return false;
    }
    // printf("i will send the pid = %d\n", msg->pid);
    msgsnd(msg_Id, (void *)msg, sizeof(struct message), IPC_NOWAIT);

    return true;
}
void SignalHandler(){

}