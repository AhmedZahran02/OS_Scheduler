
#include "headers.h"
#include "queue2.h"
#include "PriorityQueue2.h"

int msg_Id = -1;
int shm_Id = -1;
int shm_Id2 = -1;
void clearResources(int);
bool recvProcess(Process *Process);
bool initializeMsgQueue();
bool initializeShm();
void HPF();
void SRTN();
void RR();
int getCounter();

struct Queue2 processes;
struct Queue2 finishedProcesses;

void handler(int signum);
void handler2(int signum);

int main(int argc, char *argv[])
{
    printf("Scheduler is starting ...\n");
    initClk();
    signal(SIGUSR1, handler);
    signal(SIGINT, clearResources);
    shm_Id2 = shmget(CONNKEY + 1, sizeof(Process), 0666 | IPC_CREAT);
    processes = createQueue();
    finishedProcesses = createQueue();
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
    // printf("process %d real id = %d \n", process->id, process->realID);
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

    Process *shmCurrPrc;
    Process runningPrc;
    Process tempPrc;
    struct PriorityQueue2 AvilablPros;
    shmCurrPrc = (Process *)shmat(shm_Id2, (void *)0, 0);
    AvilablPros = create();
    runningPrc.realID = -1;
    *shmCurrPrc = runningPrc;
    while (1)
    {
        /* code */
        while (!isEmpty(&processes))
        {
            Process tempPrc2 = dequeue(&processes);
            insert(&AvilablPros, tempPrc2.Priority, tempPrc2);
        }
        if (shmCurrPrc->realID == -1)
        {
            if (AvilablPros.count > 0)
            {

                runningPrc = dequeue2(&AvilablPros);
                *shmCurrPrc = runningPrc;
                if (runningPrc.realID == -1)
                {
                    shmCurrPrc->realID = StartProcess(&runningPrc);
                }
                else
                {
                    //                    printf("process %d cont at time %d \n", runningPrc.id, getClk());
                    //                    kill(runningPrc.realID, SIGCONT);
                    ContinueProcess(&runningPrc);
                }
            }
        }
        else if (shmCurrPrc->remRunTime == 0)
        {

            tempPrc = *shmCurrPrc;
            enqueue(&finishedProcesses, tempPrc);
            //                printf("process %d finished at time %d \n", shmCurrPrc->id, getClk());
            FinishProcess(shmCurrPrc);
            shmCurrPrc->realID = -1;
        }
    }
}

void SRTN()
{
    Process *shmCurrProcess;
    Process currProcess;
    Process tempProcess;
    struct PriorityQueue2 currProcesses;

    // printf("process is added to system at time 3333\n");
    shmCurrProcess = (Process *)shmat(shm_Id2, (void *)0, 0);
    currProcesses = create();
    // finishedProcesses = createQueue();
    currProcess.realID = -1;
    *shmCurrProcess = currProcess;

    while (1)
    {
        // printf("process is added to system at time 4444\n");
        while (!isEmpty(&processes))
        {
            // printf("process is added to system at time 555\n");

            tempProcess = processes.front->data;
            dequeue(&processes);
            // printf("process %d is added to system at time %d with realid = %d \n", tempProcess.id, getClk(), tempProcess.realID);
            insert(&currProcesses, tempProcess.remRunTime, tempProcess);
        }

        // int currTime = getClk();
        // printf("shmCurrProcess->realID = %d \n", shmCurrProcess->realID);
        if (shmCurrProcess->realID == -1)
        {
            // printf("first stage \n");
            if (currProcesses.count > 0)
            {
                // printf("%d \n", currProcesses.count);
                //  printf("second stage \n");

                currProcess = currProcesses.front->data;
                // printf("pid %d \n", currProcess.id);
                dequeue2(&currProcesses);
                *shmCurrProcess = currProcess;

                // printf("lol real id %d \n", currProcess.realID);
                if (currProcess.realID == -1)
                {
                    // printf("third stage \n");

                    // printf("Loooool2 \n");
                    // shmCurrProcess->startingTime = getClk();
                    shmCurrProcess->realID = StartProcess(&currProcess);
                }
                else
                {
                    // resume this procces by send signal to it by the pid
                    ContinueProcess(&currProcess);
                }
            }
        }
        else
        {
            if (shmCurrProcess->remRunTime == 0)
            {
                tempProcess.id = shmCurrProcess->id;
                tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
                tempProcess.finishTime = shmCurrProcess->finishTime;
                tempProcess.Priority = shmCurrProcess->Priority;
                tempProcess.realID = shmCurrProcess->realID;
                tempProcess.remRunTime = shmCurrProcess->remRunTime;
                tempProcess.startingTime = shmCurrProcess->startingTime;
                tempProcess.runTime = shmCurrProcess->runTime;
                enqueue(&finishedProcesses, tempProcess);
                //                printf("process %d finished at time %d \n", shmCurrProcess->id, getClk());
                FinishProcess(shmCurrProcess);
                shmCurrProcess->realID = -1;

                // if (currProcesses.count > 0)
                // {
                //     currProcess = currProcesses.front->data;
                //     *shmCurrProcess = currProcess;
                //     dequeue2(&currProcesses);

                //     if (currProcess.realID == -1)
                //     {
                //         shmCurrProcess->startingTime = getClk();
                //         printf("process %d started at time %d ", tempProcess.id, getClk());

                //         int Process_Id = fork();
                //         if (Process_Id == 0)
                //         {
                //             system("gcc process.c -o process.out");
                //             execl("process.out", "process.c", currProcess.remRunTime, NULL);
                //         }
                //         currProcess.realID = Process_Id;
                //     }
                //     else
                //     {
                //         // resume this procces by send signal to it by the pid
                //         kill(SIGCONT, currProcess.realID);
                //     }
                // }
                // else
                // {
                //     shmCurrProcess->realID = -1;
                // }
            }
            else
            {
                if (currProcesses.count > 0)
                {
                    currProcess = currProcesses.front->data;
                    if (shmCurrProcess->remRunTime > currProcess.remRunTime)
                    {

                        // stop the current process and create new on if runtime = remruntime
                        //                        printf("process %d stoped at time %d \n", shmCurrProcess->id, getClk());
                        // dequeue2(&currProcesses);
                        //                        kill(shmCurrProcess->realID, SIGSTOP);
                        StopProcess(shmCurrProcess);

                        tempProcess.id = shmCurrProcess->id;
                        tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
                        tempProcess.finishTime = shmCurrProcess->finishTime;
                        tempProcess.Priority = shmCurrProcess->Priority;
                        tempProcess.realID = shmCurrProcess->realID;
                        tempProcess.remRunTime = shmCurrProcess->remRunTime;
                        tempProcess.startingTime = shmCurrProcess->startingTime;
                        tempProcess.runTime = shmCurrProcess->runTime;

                        insert(&currProcesses, tempProcess.remRunTime, tempProcess);
                        shmCurrProcess->realID = -1;

                        // if (currProcess.realID == -1)
                        // {
                        //     shmCurrProcess->startingTime = getClk();
                        //     printf("process %d started at time %d ", tempProcess.id, getClk());

                        //     int Process_Id = fork();
                        //     if (Process_Id == 0)
                        //     {
                        //         system("gcc process.c -o process.out");
                        //         execl("process.out", "process.c", NULL);
                        //     }
                        //     currProcess.realID = Process_Id;
                        // }
                        // else
                        // {
                        //     // resume this procces by send signal to it by the pid
                        //     printf("process %d cont at time %d ", currProcess.id, getClk());

                        //     kill(SIGCONT, currProcess.realID);
                        // }
                    }
                }
            }
            // printf("%d \n", shmCurrProcess->realID);
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

void RR(int quantum)
{

    Process *shmCurrProcess;
    Process currentProcess;
    // Getting the current process
    shmCurrProcess = (Process *)shmat(shm_Id2, (void *)0, 0);
    shmCurrProcess->realID = -1;
    int last_start = -1;
    while (true)
    {
        if (shmCurrProcess->realID == -1)
        { // No current process is running
            if (!isEmpty(&processes))
            {
                Process Current_Process = dequeue(&processes);
                if (Current_Process.startingTime == -1)
                { // If it's the first time to get scheduled
                    last_start = Current_Process.remRunTime;
                    StartProcess(&Current_Process);
                    *shmCurrProcess = Current_Process;
                }
                else
                { // It started before so let's make it continue
                    ContinueProcess(&Current_Process);
                    last_start = Current_Process.remRunTime;
                    *shmCurrProcess = Current_Process;
                }
            }
        }
        else
        {
            if (!shmCurrProcess->remRunTime)
            { // Process Finished
                FinishProcess(shmCurrProcess);
                enqueue(&finishedProcesses, *shmCurrProcess);
                shmCurrProcess->realID = -1;
            }
            else if (last_start - shmCurrProcess->remRunTime >= quantum && !isEmpty(&processes))
            { // If it still didn't finish but Preemption will occur
                Process Cur_Process = *shmCurrProcess;
                StopProcess(shmCurrProcess);
                if (shmCurrProcess->remRunTime)
                    enqueue(&processes, Cur_Process);
            }
        }
    }
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    printf("Scheduler terminating!\n");
    shmctl(shm_Id2, IPC_RMID, NULL);
    // killpg(getpgrp(), SIGKILL);
    // kill(getpid(), SIGKILL);
    exit(0);
}