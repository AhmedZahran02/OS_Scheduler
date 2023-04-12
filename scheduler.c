#include "headers.h"
#include "queue2.h"

int msg_Id = -1;
int shm_Id = -1;
void clearResources(int);
bool recvProcess(Process *Process);
bool initializeMsgQueue();
bool initializeShm();
void HPF();
void SRTN();
void RR();
int getCounter();

struct Queue2 processes;

void handler(int signum);

int main(int argc, char *argv[])
{
    printf("Scheduler is starting ...\n");
    initClk();
    signal(SIGUSR1, handler);
    processes = createQueue();
    initializeMsgQueue();
    initializeShm();
    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    int scheduleType = *argv[1];
    int scheduleArgument = *argv[2];

    switch (scheduleType)
    {
    case 1:
        HPF();
        break;
    case 2:
        SRTN();
        break;
    case 3:
        RR(scheduleArgument);
        break;
    }

    // habd zone
    while (1)
    {
        // donot kill my clock plz
    }
    // end of habd zone

    destroyClk(true);
}

bool initializeMsgQueue()
{
    if ((msg_Id = msgget(CONNKEY, 0666 | IPC_CREAT)) == -1)
    {
        printf("failled to initialize msg queue");
        return false;
    }
    return true;
}

bool initializeShm()
{
    if ((shm_Id = shmget(CONNKEY, 4, 0666 | IPC_CREAT)) == -1)
    {
        printf("failled to initialize shared memory");
        return false;
    }
    return true;
}

bool recvProcess(Process *process)
{
    if (msg_Id == -1)
    {
        return false;
    }
    msgrcv(msg_Id, (void *)process, sizeof(process), 0, !IPC_NOWAIT);
    return true;
}

int getCounter()
{
    int *counterAddress = (int *)shmat(shm_Id, (void *)0, 0);
    if ((long)counterAddress == -1)
    {
        printf("Error in attaching the shm of the counter");
        return false;
    }
    return (int)*counterAddress;
}

void handler(int signum)
{
    int cnt = getCounter();
    for (int i = 0; i < cnt; i++)
    {
        Process tempProcess;
        recvProcess(&tempProcess);
        enqueue(&processes, tempProcess);
    }
}

void HPF()
{
}

void SRTN()
{
}

void RR()
{
}