
#include "headers.h"
#include "queue2.h"
#include "PriorityQueue2.h"

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
Process *shmCurrProcess;

struct PriorityQueue2 currProcesses;
Process currProcess;
Process tempProcess;

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

    // switch (scheduleType)
    // {
    // case 1:
    //     HPF();
    //     break;
    // case 2:
    //     SRTN();
    //     break;
    // case 3:
    //     RR(scheduleArgument);
    //     break;
    // }

    // habd zone
    // printf("process is added to system at time \n");

    if (scheduleType == 2)
    {
        // printf("process is added to system at time 3333\n");
        int shm_Id = shmget(CONNKEY + 1, 40, 0666 | IPC_CREAT);
        shmCurrProcess = (Process *)shmat(shm_Id, (void *)0, 0);
        currProcesses = create();
        // finishedProcesses = createQueue();
        currProcess.id = -1;
        *shmCurrProcess = currProcess;

        while (1)
        {
            // printf("process is added to system at time 4444\n");

            while (!isEmpty(&processes))
            {
                // printf("process is added to system at time 555\n");

                tempProcess = processes.front->data;
                dequeue(&processes);
                printf("process %d is added to system at time %d with realid = %d \n", tempProcess.id, getClk(), tempProcess.realID);
                insert(&currProcesses, currProcess.remRunTime, tempProcess);
            }

            int currTime = getClk();
            if (shmCurrProcess->realID == -1)
            {

                if (currProcesses.count > 0)
                {
                    // printf("%d \n", currProcesses.count);

                    currProcess = currProcesses.front->data;
                    dequeue2(&currProcesses);
                    *shmCurrProcess = currProcess;

                    // printf("lol real id %d \n", currProcess.realID);
                    if (currProcess.realID == -1)
                    {
                        // printf("Loooool2 \n");
                        // shmCurrProcess->startingTime = getClk();
                        printf("process %d started at time %d \n", tempProcess.id, getClk());

                        int Process_Id = fork();
                        if (Process_Id == 0)
                        {
                            system("gcc process.c -o process.out");
                            execl("process.out", "process.c", currProcess.remRunTime, NULL);
                        }
                        currProcess.realID = Process_Id;
                    }
                    else
                    {
                        // resume this procces by send signal to it by the pid
                        kill(SIGCONT, currProcess.realID);
                    }
                }
            }
            else
            {
                // if (shmCurrProcess->remRunTime == 0)
                // {

                //     tempProcess.id = shmCurrProcess->id;
                //     tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
                //     tempProcess.finishTime = shmCurrProcess->finishTime;
                //     tempProcess.Priority = shmCurrProcess->Priority;
                //     tempProcess.realID = shmCurrProcess->realID;
                //     tempProcess.remRunTime = shmCurrProcess->remRunTime;
                //     tempProcess.startingTime = shmCurrProcess->startingTime;
                //     tempProcess.runTime = shmCurrProcess->runTime;
                //     if (currProcesses.count > 0)
                //     {
                //         currProcess = currProcesses.front->data;
                //         *shmCurrProcess = currProcess;
                //         dequeue(&currProcesses);

                //         if (currProcess.realID == -1)
                //         {
                //             shmCurrProcess->startingTime = getClk();
                //             printf("process %d started at time %d ", tempProcess.id, getClk());

                //             int Process_Id = fork();
                //             if (Process_Id == 0)
                //             {
                //                 system("gcc process.c -o process.out");
                //                 execl("process.out", "process.c", currProcess.remRunTime, NULL);
                //             }
                //             currProcess.realID = Process_Id;
                //         }
                //         else
                //         {
                //             // resume this procces by send signal to it by the pid
                //             kill(SIGCONT, currProcess.realID);
                //         }
                //     }
                // }
                // else
                // {
                //     if (currProcesses.count > 0)
                //     {
                //         currProcess = currProcesses.front->data;
                //         if (shmCurrProcess->remRunTime > currProcess.remRunTime)
                //         {
                //             // stop the current process and create new on if runtime = remruntime
                //             printf("process %d stoped at time %d ", shmCurrProcess->id, getClk());

                //             kill(SIGSTOP, shmCurrProcess->realID);

                //             tempProcess.id = shmCurrProcess->id;
                //             tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
                //             tempProcess.finishTime = shmCurrProcess->finishTime;
                //             tempProcess.Priority = shmCurrProcess->Priority;
                //             tempProcess.realID = shmCurrProcess->realID;
                //             tempProcess.remRunTime = shmCurrProcess->remRunTime;
                //             tempProcess.startingTime = shmCurrProcess->startingTime;
                //             tempProcess.runTime = shmCurrProcess->runTime;

                //             insert(&currProcesses, tempProcess.remRunTime, tempProcess);
                //             if (currProcess.realID == -1)
                //             {
                //                 shmCurrProcess->startingTime = getClk();
                //                 printf("process %d started at time %d ", tempProcess.id, getClk());

                //                 int Process_Id = fork();
                //                 if (Process_Id == 0)
                //                 {
                //                     system("gcc process.c -o process.out");
                //                     execl("process.out", "process.c", currProcess.remRunTime, NULL);
                //                 }
                //                 currProcess.realID = Process_Id;
                //             }
                //             else
                //             {
                //                 // resume this procces by send signal to it by the pid
                //                 printf("process %d cont at time %d ", currProcess.id, getClk());

                //                 kill(SIGCONT, currProcess.realID);
                //             }
                //         }
                //     }
                // }
            }
        }

        /*

        there is a shared memory that contains the remaining time of running process and its id
        this memory get updated in process.c
        there is a variable contains the remaining time of the running process
        you have array of structs of all processes
        you have array of structs of current processes in the system

        get the current clock
        loop on all processes to push the ones that arrival time less than current time
        loop on current processes to get the min running time one
        if current running process equall null {
            run the choosed pro
        }

        int clk = getclk();

        loop on the array

        get the min remaining time

        */
    }
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
    msgrcv(msg_Id, (void *)process, sizeof(Process), 0, !IPC_NOWAIT);
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