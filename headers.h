#pragma once
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "string.h"
#include <math.h>
#include <time.h>

typedef short bool;
typedef struct Process Process;
typedef struct ListNode ListNode;

#define true 1
#define false 0

#define SHKEY 300
#define CONNKEY 400
#define CLRPKEY 500

struct Process
{
    long int mtype;
    int id;
    int arrivalTime;
    int runTime;
    int Priority;
    int remRunTime;   // initially = runTime
    int realID;       // initially = -1
    int startingTime; // initially = -1
    int finishTime;   // initially = -1
    int memSize;
    int startMemLoc;
    int resumeTime;
    int previousRem ;
};

struct message
{
    int pid;
};

struct ScheduleType
{
    int type;
    int parameter;
};

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

int min(int x, int y)
{
    if (x < y)
        return x;
    return y;
}

int max(int x, int y)
{
    if (x > y)
        return x;
    return y;
}

Process createProcess(int id, int arrival, int runTime, int P, int mem)
{
    Process process;
    process.id = id;
    process.arrivalTime = arrival;
    process.runTime = runTime;
    process.Priority = P;
    process.finishTime = -1;
    process.startingTime = -1;
    process.remRunTime = runTime;
    process.realID = -1;
    process.memSize = mem;

    return process;
}

void ProcessFinished(Process process)
{
    FILE *file_ptr;
    file_ptr = fopen("scheduler.log", "mode");
    if (file_ptr == NULL)
    {
        printf("Error! in opening Scheduler log file");
        exit(1);
    }
    char str[100];
    strcpy(str, "At time");
    const int time_size = 10;
    fwrite(str, 1, sizeof(str), file_ptr);
    snprintf(str, time_size, "%d", getClk());
    strcpy(str, " process \n");
    fclose(file_ptr);
}

FILE *OpenFile(char *fileName)
{
    FILE *out_file = fopen(fileName, "a"); // append
    return out_file;
}

void CloseFile(FILE *fp)
{
    fclose(fp);
}

int StartProcess(Process *P)
{
    int wait = getClk() - P->arrivalTime;
    FILE *fptr = OpenFile("scheduler.log");
//    printf("At time %d process %d STARTED arr: %d total: %d remaining: %d wait:%d \n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait);
    fprintf(fptr, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait);
    CloseFile(fptr);
    int Process_Id = fork();
    if (Process_Id == 0)
    {
        system("gcc process.c -o process.out");
        execl("process.out", "process.c", NULL);
    }
    else
        P->realID = Process_Id;
    return Process_Id;
}

Process * FinishProcess(Process *P)
{
    P->finishTime = getClk();
    int wait = P->startingTime - P->arrivalTime;
    int TA = P->finishTime - P->arrivalTime;
    double WTA = (double)TA / P->runTime;
    FILE *fptr = OpenFile("scheduler.log");
//    printf("At time %d process %d FINISHED arr: %d total: %d remaining: %d wait: %d TA: %d WTA : %.*f\n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait, TA, 2, WTA);
    fprintf(fptr, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.*f\n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait, TA, 2, WTA);
    CloseFile(fptr);
    return P;
}

Process *StopProcess(Process *P)
{
    if (P->realID != -1)
    {
        kill(P->realID, SIGSTOP);
        int wait = P->startingTime - P->arrivalTime;
        FILE *fptr = OpenFile("scheduler.log");
//        printf("At time %d process %d STOPPED arr: %d total: %d remaining: %d wait: %d \n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait);
//        usleep(100*1000);
        fprintf(fptr, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait);
        CloseFile(fptr);
        kill(P->realID, SIGSTOP);
        P->realID = -1;
    }
    return P ;
}
Process *StopProcessLingo(Process *P ,int val)
{
    if (P->realID != -1)
    {
        kill(P->realID, SIGSTOP);
        int wait = P->startingTime - P->arrivalTime;
        FILE *fptr = OpenFile("scheduler.log");
//        printf("At time %d process %d STOPPED arr: %d total: %d remaining: %d wait: %d \n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait);
        fprintf(fptr, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", val, P->id, P->arrivalTime, P->runTime, P->remRunTime, wait);
        CloseFile(fptr);
        kill(P->realID, SIGSTOP);
        P->realID = -1;
    }
    return P ;
}

void ContinueProcess(Process *P)
{
    if (P->realID != -1)
    {
        FILE *fptr = OpenFile("scheduler.log");
        fprintf(fptr, "At time %d process %d resumed arr %d total %d remain %d\n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime);
        CloseFile(fptr);
        kill(P->realID, SIGCONT);
        int wait = P->startingTime - P->arrivalTime;
//        printf("At time %d process %d CONTINUED arr: %d total: %d remain: %d wait:%d  \n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait);
    }
}
void ContinueProcessLingo(Process *P , int val)
{
    if (P->realID != -1)
    {
        FILE *fptr = OpenFile("scheduler.log");
        fprintf(fptr, "At time %d process %d resumed arr %d total %d remain %d\n", val, P->id, P->arrivalTime, P->runTime, P->remRunTime);
        CloseFile(fptr);
        kill(P->realID, SIGCONT);
        int wait = P->startingTime - P->arrivalTime;
//        printf("At time %d process %d CONTINUED arr: %d total: %d remain: %d wait:%d  \n", getClk(), P->id, P->arrivalTime, P->runTime, P->remRunTime, wait);
    }
}
