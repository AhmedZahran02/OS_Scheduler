#include "PriorityQueue2.h"
#include "queue2.h"
#include "headers.h"

/* Modify this file as needed*/
// int remainingtime;

int msgq_id, rec_val;
void *shmAddrCnt;
Process *shmCurrProcess;

struct PriorityQueue2 currProcesses;
struct Queue2 finishedProcesses;
Process currProcess;
Process tempProcess;

void handler(int signum);

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGUSR1, handler);
    currProcesses = create();
    finishedProcesses = createQueue();
    currProcess.id = -1;

    msgq_id = msgget(CONNKEY, 0666 | IPC_CREAT);
    shmAddrCnt = shmat(CONNKEY, (void *)0, 0);
    shmCurrProcess = shmat(CONNKEY + 1, (void *)0, 0);
    if (shmCurrProcess == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    shmCurrProcess->id = -1;

    while (1)
    {

        int currTime = getClk();
        if (shmCurrProcess->id == -1)
        {
            if (currProcesses.count > 0)
            {
                currProcess = currProcesses.front->data;
                *shmCurrProcess = currProcess;
                dequeue(&currProcesses);
                if (currProcess.realID == -1)
                {
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
                }
            }
        }
        else
        {
            if (shmCurrProcess->remRunTime == 0)
            {

                tempProcess.id = shmCurrProcess->id;
                tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
                //  cont.
                //
                //
                //
                enqueue(&finishedProcesses, tempProcess);
                if (currProcesses.count > 0)
                {
                    currProcess = currProcesses.front->data;
                    *shmCurrProcess = currProcess;
                    dequeue(&currProcesses);

                    if (currProcess.realID == -1)
                    {
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
                    }
                }
            }
            else
            {
                if (currProcesses.count > 0)
                {
                    currProcess = currProcesses.front->data;
                    if (shmCurrProcess->remRunTime > currProcess.remRunTime)
                    {
                        // stop the current process and create new on if runtime = remruntime
                        tempProcess.id = shmCurrProcess->id;
                        tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
                        //  cont.
                        //
                        //
                        //
                        enqueue(&currProcesses, tempProcess);
                        if (currProcess.realID == -1)
                        {
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
                        }
                    }
                }
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

    // destroyClk(false);

    return 0;
}

void handler(int signum)
{
    int cnt = (int *)shmAddrCnt;
    for (int i = 0; i < cnt; i++)
    {
        Process tempProcess;
        rec_val = msgrcv(msgq_id, &(tempProcess), sizeof(tempProcess), 0, !IPC_NOWAIT);
    }
}