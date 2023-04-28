#include "headers.h"
#include "queue2.h"
#include "PriorityQueue2.h"

int msg_Id = -1;
int msg_Id2 = -1;
int shm_Id = -1;
int shm_Id2 = -1;
Process *shmCurrProcess;

int numOfProcesses;
void clearResources2(int);
void cleanProcessResources(int pid);
bool recvProcess(Process *Process);
bool initializeMsgQueue();
bool initializeMsgQueue2();
bool initializeShm();
bool initializeShm2();
void HPF();
void SRTN();
void RR(int qunatum);
int getCounter();
void genPrefFile();
double calculateSD(double *data, double *avgWTA);

struct Queue2 readyQueue;
struct Queue2 finishedProcesses;

void handler(int signum);
void cleanprocess(int signum);

int main(int argc, char *argv[])
{
    printf("Scheduler is starting ...\n");
    initClk();
    signal(SIGUSR1, handler);
    signal(SIGINT, clearResources2);
    signal(SIGUSR2, cleanprocess);
    readyQueue = createQueue();
    finishedProcesses = createQueue();
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

    FILE *out_file = fopen("scheduler.log", "w"); // write only
    fprintf(out_file, "#At time x process y state arr w total z remain y wait k\n");
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
        enqueue(&readyQueue, tempProcess);
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

    cleanProcessResources(msg->pid);
    // printf("i recieved the pid = %d\n", msg->pid);

    return;
}

void cleanProcessResources(int pid)
{
    printf("i recieved the pid = %d\n", pid);
    // remove from ready queue to finished using pid detail
    // put final details in process struct
    // free memory used by that pid
    // remove the finishing in the 3 scheduling
    Process tempProcess;

    FinishProcess(shmCurrProcess);
    tempProcess.id = shmCurrProcess->id;
    tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
    tempProcess.finishTime = shmCurrProcess->finishTime;
    tempProcess.Priority = shmCurrProcess->Priority;
    tempProcess.realID = shmCurrProcess->realID;
    tempProcess.remRunTime = shmCurrProcess->remRunTime;
    tempProcess.startingTime = shmCurrProcess->startingTime;
    tempProcess.runTime = shmCurrProcess->runTime;
    enqueue(&finishedProcesses, tempProcess);
    shmCurrProcess->realID = -1;
}

void HPF()
{
    Process runningPrc;
    Process tempPrc;
    struct PriorityQueue2 AvilablPros;
    AvilablPros = create();
    runningPrc.realID = -1;
    *shmCurrProcess = runningPrc;
    while (1)
    {
        if (finishedProcesses.count == numOfProcesses)
        {
            break;
        }
        while (!isEmpty(&readyQueue))
        {
            Process tempPrc2 = dequeue(&readyQueue);
            insert(&AvilablPros, tempPrc2.Priority, tempPrc2);
        }
        if (shmCurrProcess->realID == -1)
        {
            if (AvilablPros.count > 0)
            {

                runningPrc = dequeue2(&AvilablPros);
                *shmCurrProcess = runningPrc;
                if (runningPrc.realID == -1)
                {
                    shmCurrProcess->realID = StartProcess(&runningPrc);
                }
                else
                {
                    ContinueProcess(&runningPrc);
                }
            }
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

    while (1)
    {
        //  printf("lol \n");
        if (finishedProcesses.count == numOfProcesses)
        {
            break;
        }
        while (!isEmpty(&readyQueue))
        {
            tempProcess = readyQueue.front->data;
            dequeue(&readyQueue);
            insert(&readyPQueue, tempProcess.remRunTime, tempProcess);
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
                    StopProcess(shmCurrProcess);
                    tempProcess.id = shmCurrProcess->id;
                    tempProcess.arrivalTime = shmCurrProcess->arrivalTime;
                    tempProcess.finishTime = shmCurrProcess->finishTime;
                    tempProcess.Priority = shmCurrProcess->Priority;
                    tempProcess.remRunTime = shmCurrProcess->remRunTime;
                    tempProcess.startingTime = shmCurrProcess->startingTime;
                    tempProcess.runTime = shmCurrProcess->runTime;
                    insert(&readyPQueue, tempProcess.remRunTime, tempProcess);
                    shmCurrProcess->realID = -1;
                }
            }
        }
    }
}

void RR(int quantum)
{
    Process currentProcess;
    // Getting the current process
    shmCurrProcess = (Process *)shmat(shm_Id2, (void *)0, 0);
    shmCurrProcess->realID = -1;
    int last_start = -1;
    while (1)
    {
        if (finishedProcesses.count == numOfProcesses)
        {
            break;
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
            if (last_start - shmCurrProcess->remRunTime >= quantum && !isEmpty(&readyQueue))
            { // If it still didn't finish but Preemption will occur
                Process Cur_Process = *shmCurrProcess;
                StopProcess(shmCurrProcess);
                if (shmCurrProcess->remRunTime)
                    enqueue(&readyQueue, Cur_Process);
            }
        }
    }
}

void genPrefFile()
{
    double *data;
    double WTsum = 0;
    data = (double *)malloc(numOfProcesses * sizeof(double));
    int i = 0;
    while (!isEmpty(&finishedProcesses))
    {
        Process tempProcess;
        tempProcess = dequeue(&finishedProcesses);
        data[i] = ((double)tempProcess.finishTime - tempProcess.arrivalTime) / tempProcess.runTime;
        // printf("%f %d %d\n", data[i], tempProcess.finishTime, tempProcess.arrivalTime);
        WTsum += (tempProcess.startingTime - tempProcess.arrivalTime);
        i++;
    }
    double avgWTA;
    double sdWTA = calculateSD(data, &avgWTA);
    double avgWT = WTsum / numOfProcesses;

    FILE *out_file2 = fopen("scheduler.pref", "w"); // write only
    fprintf(out_file2, "CPU Utilization = %.*f %%\nAVGWait = %.*f\nAVGWTA = %.*f\nstdWTA = %.*f\n", 2, 100.0, 2, avgWT, 2, avgWTA, 2, sdWTA);
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