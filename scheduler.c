#include "headers.h"
#include "queue2.h"
#include "PriorityQueue2.h"
#include "linkedlist.h"

int msg_Id = -1;
int msg_Id2 = -1;
int shm_Id = -1;
int shm_Id2 = -1;
int memType;
int signalCheck = 0;
Process signalProcess;
Process *shmCurrProcess;
int finishedProcessesCount = 0;

int numOfProcesses;
ListNode *freeMem;

void clearResources2(int);
void cleanProcessResources(int pid);
void nextSecondWaiting(int *lastSecond) ;
bool recvProcess(Process *Process);
bool initializeMsgQueue();
bool initializeMsgQueue2();
bool initializeShm();
bool initializeShm2();
void HPF();
void SRTN();
void RR(int qunatum);
bool occupyMemory(Process * p);
void releaseMem(Process * p);
bool FF(Process *process);
bool BMA(Process *process);
void releaseMemFF(int ,int, int);
void releaseMemBMA(int , int, int );
void DeleteMemory(ListNode *reqLoc, int process_id);
ListNode *splitMemoryBuddy(int process_size, ListNode *freePart);
int getCounter();
void genPrefFile();
double calculateSD(double *data, double *avgWTA);

struct Queue2 waitingQueue;
struct Queue2 tempWaitingQueue;

struct Queue2 readyQueue;
struct Queue2 finishedProcesses;

void handler(int signum);
void cleanprocess(int signum);
double cpuUtilization();
void getBrother(int mystart, int myend, int *st, int *en);

int main(int argc, char *argv[])
{
    printf("Scheduler is starting ...\n");
    initClk();
    signal(SIGUSR1, handler);
    signal(SIGINT, clearResources2);
    signal(SIGUSR2, cleanprocess);
    readyQueue = createQueue();
    waitingQueue = createQueue();
    tempWaitingQueue = createQueue();

    finishedProcesses = createQueue();
    freeMem = createHead(0, 1023);
    // printf("%d %d", freeMem->start, freeMem->end);
    initializeMsgQueue();
    initializeMsgQueue2();
    initializeShm();
    initializeShm2();
    shmCurrProcess = (Process *)shmat(shm_Id2, (void *)0, 0);

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    int scheduleType = *argv[1];
    int scheduleArgument = *argv[2];
    numOfProcesses = *argv[3];
    memType = *argv[4];

    FILE *out_file = fopen("scheduler.log", "w"); // write only
    fprintf(out_file, "#At time x process y state arr w total z remain y wait k\n");
    CloseFile(out_file);

    out_file = fopen("memory.log", "w"); // write only
    fprintf(out_file, "#At time x allocated y bytes for process z from i to j\n");
    CloseFile(out_file);

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

    genPrefFile();
    clearResources2(0);
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

bool initializeMsgQueue2()
{
    if ((msg_Id2 = msgget(CLRPKEY, 0666 | IPC_CREAT)) == -1)
    {
        printf("failled to initialize msg queue 2");
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

bool initializeShm2()
{
    if ((shm_Id2 = shmget(CONNKEY + 1, 40, 0666 | IPC_CREAT)) == -1)
    {
        printf("failled to initialize shared memory 2");
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
        enqueue(&waitingQueue, tempProcess);
    }
}

void cleanprocess(int signum)
{

    if (msg_Id2 == -1)
    {
        printf("failed to clean process\n");
        return;
    }
    struct message *msg = malloc(sizeof(struct message));
    msgrcv(msg_Id2, (void *)msg, sizeof(struct message), 0, !IPC_NOWAIT);
    printf("loool schdeuler receive signal to finsish with id = %d , realId = %d \n", shmCurrProcess->id, shmCurrProcess->realID);

    cleanProcessResources(msg->pid);
    // printf("i recieved the pid = %d\n", msg->pid);

    return;
}

void cleanProcessResources(int pid)
{
    // printf("i recieved the pid = %d\n", pid);
    // remove from ready queue to finished using pid detail
    // put final details in process struct
    // free memory used by that pid
    // remove the finishing in the 3 scheduling


    Process tempProcess;
    if (shmCurrProcess->realID == -1)
    {
        printf("ahhhhhhhhhhhhhhhhhhhhhh \n");
    }
    else
    {
        printf("loool schdeuler receive signal to finsish with id = %d , realId = %d \n", shmCurrProcess->id, shmCurrProcess->realID);
    }
    shmCurrProcess = FinishProcess(shmCurrProcess);
    signalProcess.id = shmCurrProcess->id;
    signalProcess.arrivalTime = shmCurrProcess->arrivalTime;
    signalProcess.finishTime = shmCurrProcess->finishTime;
    signalProcess.Priority = shmCurrProcess->Priority;
    signalProcess.realID = shmCurrProcess->realID;
    signalProcess.remRunTime = shmCurrProcess->remRunTime;
    signalProcess.startingTime = shmCurrProcess->startingTime;
    signalProcess.runTime = shmCurrProcess->runTime;
    signalProcess.memSize = shmCurrProcess->memSize;
    signalProcess.startMemLoc = shmCurrProcess->startMemLoc;
    releaseMem(&signalProcess);
    enqueue(&finishedProcesses, signalProcess);
    shmCurrProcess->realID = -1;
    finishedProcessesCount++;
}

void HPF()
{
    Process runningPrc;
    Process tempPrc;
    struct PriorityQueue2 AvilablPros;
    AvilablPros = create();
    runningPrc.realID = -1;
    *shmCurrProcess = runningPrc;
    while (finishedProcesses.count != numOfProcesses)
    {
//        while (!isEmpty(&readyQueue))
//        {
//            Process tempPrc2 = dequeue(&readyQueue);
//        }
        Process tempProcess;
        while (!isEmpty(&waitingQueue))
        {
            tempProcess = waitingQueue.front->data;
            dequeue(&waitingQueue);
            if(occupyMemory(&tempProcess)){ // If there is free memory suitable for the process
                insert(&AvilablPros, tempProcess.Priority, tempProcess);
            }else{ // Otherwise put it in the waiting till there is free space for it
                enqueue(&tempWaitingQueue, tempProcess);
            }
        }

        while (!isEmpty(&tempWaitingQueue))
        {
            tempProcess = tempWaitingQueue.front->data;
            dequeue(&tempWaitingQueue);
            enqueue(&waitingQueue, tempProcess);
        }
        if (shmCurrProcess->realID == -1)
        { // If there is no current process running
            if (AvilablPros.count > 0)
            {// If there are ready processes
                runningPrc = dequeue2(&AvilablPros);
                *shmCurrProcess = runningPrc;
                if (runningPrc.realID == -1)
                { // If it's the first time to start
                    shmCurrProcess->realID = StartProcess(&runningPrc);
                }
                else
                {// Otherwise , it started before so let it continue (CONTEXT SWITCHING)
                    ContinueProcess(&runningPrc);
                }
            }
        }

        if (signalCheck == 1)
        { // If there was signal sent to kill the process
            releaseMem(&signalProcess);
            enqueue(&finishedProcesses, signalProcess);
            shmCurrProcess->realID = -1;
            signalCheck = 0;
        }
    }
}

void SRTN()
{
    Process currProcess;
    Process tempProcess;
    struct PriorityQueue2 readyPQueue;

    readyPQueue = create();
    currProcess.realID = -1;
    *shmCurrProcess = currProcess;

    while (finishedProcesses.count != numOfProcesses)
    {
        while (!isEmpty(&waitingQueue))
        {

            tempProcess = waitingQueue.front->data;
            dequeue(&waitingQueue);

            if(occupyMemory(&tempProcess))
                insert(&readyPQueue, tempProcess.remRunTime, tempProcess);
            else
                enqueue(&tempWaitingQueue, tempProcess);
        }

        while (!isEmpty(&tempWaitingQueue))
        {
            tempProcess = tempWaitingQueue.front->data;
            dequeue(&tempWaitingQueue);
            enqueue(&waitingQueue, tempProcess);
        }

        if (shmCurrProcess->realID == -1)
        {
            if (readyPQueue.count > 0)
            {
                currProcess = readyPQueue.front->data;
                dequeue2(&readyPQueue);
                *shmCurrProcess = currProcess;

                if (currProcess.realID == -1)
                {
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

            if (readyPQueue.count > 0)
            {
                currProcess = readyPQueue.front->data;
                if (shmCurrProcess->remRunTime > currProcess.remRunTime)
                {
                    tempProcess.realID = shmCurrProcess->realID;
                    shmCurrProcess =  StopProcess(shmCurrProcess);
                    tempProcess.id = shmCurrProcess->id;
                    tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
                    tempProcess.finishTime = shmCurrProcess->finishTime;
                    tempProcess.Priority = shmCurrProcess->Priority;
                    tempProcess.remRunTime = shmCurrProcess->remRunTime;
                    tempProcess.startingTime = shmCurrProcess->startingTime;
                    tempProcess.runTime = shmCurrProcess->runTime;
                    tempProcess.memSize = shmCurrProcess->memSize;
                    tempProcess.startMemLoc = shmCurrProcess->startMemLoc;
                    insert(&readyPQueue, tempProcess.remRunTime, tempProcess);
                    shmCurrProcess->realID = -1;
                }
            }
        }
        if (signalCheck == 1)
        {
            releaseMem(&signalProcess);
            enqueue(&finishedProcesses, signalProcess);
            shmCurrProcess->realID = -1;

            signalCheck = 0;
        }
    }
}

void RR(int quantum)
{
    Process currentProcess;
    int  current_time =-1;
    // Getting the current process
    shmCurrProcess = (Process *)shmat(shm_Id2, (void *)0, 0);
    shmCurrProcess->realID = -1;
    int last_start = -1;
    while (finishedProcesses.count != numOfProcesses)
    {
        Process tempProcess;
        while (!isEmpty(&waitingQueue))
        {
            tempProcess = waitingQueue.front->data;
            dequeue(&waitingQueue);
            if(occupyMemory(&tempProcess)) enqueue(&readyQueue, tempProcess);
            else enqueue(&tempWaitingQueue, tempProcess);
        }

        while (!isEmpty(&tempWaitingQueue))
        {
            tempProcess = tempWaitingQueue.front->data;
            dequeue(&tempWaitingQueue);
            enqueue(&waitingQueue, tempProcess);
        }

        if (shmCurrProcess->realID == -1)
        { // No current process is running
            if (!isEmpty(&readyQueue))
            {
                Process Current_Process = dequeue(&readyQueue);
                if (Current_Process.startingTime == -1)
                { // If it's the first time to get scheduled
                    last_start = Current_Process.remRunTime;
                    StartProcess(&Current_Process);
                    *shmCurrProcess = Current_Process;
                }
                else if (Current_Process.remRunTime > 0)
                { // It started before so let's make it continue
                    ContinueProcess(&Current_Process);
                    last_start = Current_Process.remRunTime;
                    *shmCurrProcess = Current_Process;
//                    current_time =getClk();
//                    nextSecondWaiting(&current_time);
                }
            }
        }
        else
        {
            if (last_start - shmCurrProcess->remRunTime >= quantum && shmCurrProcess->remRunTime > 0 && !isEmpty(&readyQueue))
            { // If it still didn't finish but Preemption will occur
                current_time =getClk();
                nextSecondWaiting(&current_time);
                Process Cur_Process = *shmCurrProcess;
                shmCurrProcess = StopProcess(shmCurrProcess);
                // if (shmCurrProcess->remRunTime)
                readyQueue =  *enqueue(&readyQueue, Cur_Process);
            }
        }

    }
}

bool FF(Process *process)
{
    ListNode *reqLoc;
    reqLoc = findFirstFit(freeMem, process->memSize);
    if (reqLoc != NULL) // It means the memory is not full
    {
        int st = reqLoc->start;
        int en = reqLoc->end;
        if (reqLoc->end - reqLoc->start + 1 > process->memSize)
        {
            // printf("YES \n");
            process->startMemLoc = reqLoc->start;

            freeMem = deleteNode(freeMem, reqLoc);
            printf("deleted from free memory start  %d end %d \n", st, en);
            // printf("%d %d \n", freeMem->start, freeMem->end);

            freeMem = insertSorted(freeMem, st + process->memSize, en);
            printf("reserved process ID %d start  %d end %d \n", process->id, st, st + process->memSize - 1);
            FILE *fptr = OpenFile("memory.log");
            fprintf(fptr, "At time %d allocated %d bytes for process %d from  %d to %d\n", getClk(), process->memSize, process->id, st, st + process->memSize - 1);
            CloseFile(fptr);
            printf("inserted in free memory start  %d end %d \n", st + process->memSize, en);
        }
        else
        {
            process->startMemLoc = reqLoc->start;
            freeMem = deleteNode(freeMem, reqLoc);
            printf("deleted from free memory start  %d end %d \n", st, en);
            printf("ID %d start  %d end %d \n", process->id, st, en);
            FILE *fptr = OpenFile("memory.log");
            fprintf(fptr, "At time %d allocated %d bytes for process %d from  %d to %d\n", getClk(), process->memSize, process->id, st, en);
            CloseFile(fptr);
        }
        return true;
    }
    return false; // There is no available suitable memory
}

bool BMA(Process *process)
{
    ListNode *reqLoc = findBestFit(freeMem, process->memSize);
    if (reqLoc == NULL)
        return false;
    reqLoc = splitMemoryBuddy(process->memSize, reqLoc);
    process->startMemLoc = reqLoc->start;
    DeleteMemory(reqLoc, process->id);
    return true;
}

void DeleteMemory(ListNode *reqLoc, int process_id)
{
    int st = reqLoc->start, en = reqLoc->end, memLength = en - st + 1;
    freeMem = deleteNode(freeMem, reqLoc);
    printf("deleted from free memory start  %d end %d \n", st, en);
    printf("ID %d start  %d end %d \n", process_id, st, en);
    // printf("after delete\n");
    // PrintList(freeMem);
    FILE *fptr = OpenFile("memory.log");
    fprintf(fptr, "At time %d allocated %d bytes for process %d from  %d to %d\n", getClk(), memLength, process_id, st, en);
    CloseFile(fptr);
}

ListNode *splitMemoryBuddy(int process_size, ListNode *freePart)
{
    int req_size = 1 << ((int)ceil(log2(process_size)));
    int st = freePart->start, en = freePart->end;
    int cur_size = en - st + 1;
    while (req_size < cur_size)
    {
        cur_size /= 2;
        // printf("before delete\n");
        // PrintList(freeMem);
        freeMem = deleteNode(freeMem, freePart);
        freeMem = insertSorted(freeMem, freePart->start, freePart->start + cur_size - 1);
        freeMem = insertSorted(freeMem, freePart->start + cur_size, freePart->end);
        freePart = find(freeMem, freePart->start, 0);
        // PrintList(freeMem);
        // freePart->end = en = (st + en) >> 1;
    }
    return freePart;
}

void releaseMemFF(int process_id , int memSize , int startMemLoc )
{
    ListNode *previousNode;
    ListNode *nextNode;
    int st;
    int en;
    nextNode = find(freeMem, startMemLoc + memSize, false);
    previousNode = find(freeMem, startMemLoc - 1, true);
    printf("released process ID %d start  %d end %d \n", process_id, startMemLoc, startMemLoc + memSize - 1);
    FILE *fptr = OpenFile("memory.log");
    fprintf(fptr, "At time %d freed %d bytes for process %d from  %d to %d\n", getClk(), memSize, process_id, startMemLoc, startMemLoc + memSize - 1);
    CloseFile(fptr);
    if (previousNode == NULL && nextNode == NULL)
    {
        // printf("one \n");
        freeMem = insertSorted(freeMem, startMemLoc, startMemLoc + memSize - 1);
        printf("inserted in free memory start  %d end %d \n", startMemLoc, startMemLoc + memSize - 1);
    }
    else if (previousNode == NULL && nextNode != NULL)
    {
        // printf("two \n");

        st = nextNode->start;
        en = nextNode->end;

        freeMem = deleteNode(freeMem, nextNode);
        printf("deleted from free memory start  %d end %d \n", st, en);

        freeMem = insertSorted(freeMem, startMemLoc, nextNode->end);
        printf("inserted in free memory start  %d end %d \n", startMemLoc, nextNode->end);
    }
    else if (previousNode != NULL && nextNode == NULL)
    {
        // printf("three \n");

        printf("deleted from free memory start  %d end %d \n", previousNode->start, previousNode->end);

        freeMem = deleteNode(freeMem, previousNode);

        freeMem = insertSorted(freeMem, previousNode->start, startMemLoc +memSize - 1);
        printf("inserted in free memory start  %d end %d \n", previousNode->start, startMemLoc +memSize - 1);
    }
    else
    {
        // printf("four \n");

        printf("deleted from free memory start  %d end %d \n", previousNode->start, previousNode->end);

        freeMem = deleteNode(freeMem, previousNode);
        printf("deleted from free memory start  %d end %d \n", nextNode->start, nextNode->end);

        freeMem = deleteNode(freeMem, nextNode);

        freeMem = insertSorted(freeMem, previousNode->start, nextNode->end);
        printf("inserted in free memory start  %d end %d \n", previousNode->start, nextNode->end);
    }
}

void releaseMemBMA(int process_id , int startMemLoc , int memSize)
{
    int start = startMemLoc;
    int end;
    int closer = memSize;
    int powof2 = 2;
    while (powof2 < closer)
    {
        powof2 = powof2 * 2;
    }

    FILE *fptr = OpenFile("memory.log");
    fprintf(fptr, "At time %d freed %d bytes for process %d from  %d to %d\n", getClk(), powof2,process_id, startMemLoc, startMemLoc + powof2 - 1);
    CloseFile(fptr);

    while (true)
    {
        ListNode *brother;
        int st;
        int en;
        int closer2 = 2;
        while (closer2 < closer)
        {
            closer2 = closer2 * 2;
        }
        end = start + closer2 - 1;
        getBrother(start, end, &st, &en);
        brother = find(freeMem, st, 0);
        if (brother != NULL && (brother->end - brother->start) == (end - start))
        {
            printf("Brother is start: %d end: %d\n", brother->start, brother->end);
            if (brother->start > start + closer2 - 1)
            {
                freeMem = deleteNode(freeMem, brother);
                printf("deleted from free memory start  %d end %d \n", st, en);
                end = brother->end;
                closer = (end - start + 1);
            }
            else
            {
                freeMem = deleteNode(freeMem, brother);
                printf("deleted from free memory start  %d end %d \n", st, en);
                start = brother->start;
                closer = (end - start + 1);
            }
        }
        else
        {
            freeMem = insertSorted(freeMem, start, end);
            printf("inserted in free memory start  %d end %d \n", start, end);
            PrintList(freeMem);
            break;
        }
        printf("start: %d end:  %d closer: %d\n", start, end, closer);
        PrintList(freeMem);
    }
}
void releaseMem(Process * p){
    if (memType == 1) releaseMemFF(p->id , p->memSize , p->startMemLoc);
    else releaseMemBMA(p->id , p->startMemLoc , p->memSize);
}
bool occupyMemory(Process * p){
    if (memType == 1) return FF(p);
    else if(memType ==2) return BMA(p);
}
void getBrother(int mystart, int myend, int *st, int *en)
{
    int start = -1, end = -1;
    if (((mystart & (mystart - 1)) == 0) && (((myend + 1) & myend) == 0))
    {
        if (mystart == 0)
        {
            start = myend + 1;
            end = (myend + 1) * 2 - 1;
        }
        else
        {
            start = 0;
            end = mystart - 1;
        }
    }
    else if ((mystart & (mystart - 1)) == 0)
    {
        end = mystart * 2 - 1;
        start = myend + 1;
    }
    else if (((myend + 1) & myend) == 0)
    {
        end = mystart - 1;
        start = (myend + 1) / 2;
    }
    *st = start;
    *en = end;
}

void genPrefFile()
{
    double *data;
    double WTsum = 0;
    double cpuUtil = cpuUtilization();
    data = (double *)malloc(numOfProcesses * sizeof(double));
    int i = 0;
    while (!isEmpty(&finishedProcesses))
    {
        Process tempProcess;
        tempProcess = dequeue(&finishedProcesses);
        data[i] = ((double)tempProcess.finishTime - tempProcess.arrivalTime) / tempProcess.runTime;
        printf("%f %d %d\n", data[i], tempProcess.finishTime, tempProcess.arrivalTime);
        WTsum += (tempProcess.startingTime - tempProcess.arrivalTime);
        i++;
    }
    double avgWTA;
    double sdWTA = calculateSD(data, &avgWTA);
    double avgWT = WTsum / numOfProcesses;

    FILE *out_file2 = fopen("scheduler.pref", "w"); // write only

    fprintf(out_file2, "CPU Utilization = %.*f %%\nAVGWait = %.*f\nAVGWTA = %.*f\nstdWTA = %.*f\n", 2, cpuUtil, 2, avgWT, 2, avgWTA, 2, sdWTA);
    CloseFile(out_file2);
}

double calculateSD(double *data, double *avgWTA)
{
    double sum = 0.0, mean, SD = 0.0;
    int i;
    for (i = 0; i < numOfProcesses; ++i)
    {
        sum += data[i];
    }
    // printf("test %f %d\n", sum, numOfProcesses);
    mean = sum / numOfProcesses;
    (*avgWTA) = mean;
    for (i = 0; i < numOfProcesses; ++i)
    {
        SD += pow(data[i] - mean, 2);
    }
    return sqrt(SD / numOfProcesses);
}

void clearResources2(int signum)
{
    // TODO Clears all resources in case of interruption
    shmctl(shm_Id2, IPC_RMID, NULL);
    printf("Scheduler terminating!\n");
    exit(0);
}

double cpuUtilization()
{
    int firstArrival = 0;
    int finishTime = 0;
    struct Node3 *p1 = finishedProcesses.front;
    struct Node3 *p2 = finishedProcesses.rear;
    firstArrival = p1->data.arrivalTime;
    finishTime = p2->data.finishTime;
    int runningSum = 0;
    while (p1 != p2)
    {
        runningSum += p1->data.runTime;
        p1 = p1->next;
    }
    runningSum += p1->data.runTime;
    int realTime = finishTime;
    runningSum = min(runningSum, realTime);
    double utilization = 100 * (double)(runningSum) / (realTime);
    return utilization;
}

void nextSecondWaiting(int *lastSecond)
{
    while (*lastSecond == getClk()) ;
    *lastSecond = getClk();
}