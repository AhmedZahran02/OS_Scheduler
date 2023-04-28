#include "headers.h"
#include "string.h"

int msg_Id = -1;
int shm_Id = -1;
void clearResources(int);
char **split(char *string, char *seperators, int *count);
bool sendProcess(Process *Process);
bool initializeMsgQueue();
bool initializeShm();
bool updateCounter(int counter);

Process *readFile(char *file, int *size);
struct ScheduleType getChosenScheduling();

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    int size = 0;
    Process *processArray = readFile("processes.txt", &size);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    struct ScheduleType scheduleType = getChosenScheduling();
    // 3. Initiate and create the scheduler and clock processes.
    int clk_Id = fork();
    if (clk_Id == 0)
    {
        system("gcc clk.c -o clk.out");
        execl("clk.out", "clk.c", NULL);
    }
    else if (clk_Id == -1)
    {
        printf("clock has failed initailizing");
    }

    int Scheduler_Id = fork();
    if (Scheduler_Id == 0)
    {
        system("gcc scheduler.c -o scheduler.out -lm");
        execl("scheduler.out", "scheduler.c", &(scheduleType.type), &(scheduleType.parameter), &size, NULL);
    }
    else if (Scheduler_Id == -1)
    {
        printf("Scheduler has failed initailizing");
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    // int x = getClk();
    // printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    int ind = 0;
    initializeMsgQueue();
    initializeShm();
    while (ind < size)
    {
        int x = getClk();
        int counter = 0;
        while (processArray[ind].arrivalTime == x && ind < size)
        {
            sendProcess(&processArray[ind]);
            counter++;
            ind++;
        }
        if (counter > 0)
        {
            updateCounter(counter);
            printf("i will send you a signal so you collect %d processes at clock = %d \n", counter, x);
            kill(Scheduler_Id, SIGUSR1);
        }
    }

    // 7. Clear clock resources
    // destroyClk(true);

    // habd zone
    int status;
    waitpid(-1, &status, 0);
    clearResources(0);
    // end of habd zone
}

Process *readFile(char *file, int *size)
{
    FILE *ptr;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    ptr = fopen(file, "r");

    if (NULL == ptr)
    {
        printf("file can't be opened \n");
    }

    int arraySize = -1;
    while ((read = getline(&line, &len, ptr)) != -1)
    {
        arraySize++;
    }

    ptr = fopen(file, "r");

    Process *processArray = malloc(arraySize * sizeof(Process));
    while ((read = getline(&line, &len, ptr)) != -1)
    {
        // printf("%s", line);
        if ((*size) > 0)
        {
            int count;
            char **output = split(line, "\t", &count);
            int id = atoi(output[0]), arrival = atoi(output[1]), runTime = atoi(output[2]), priority = atoi(output[3]), mem = atoi(output[4]);
            processArray[(*size) - 1] = createProcess(id, arrival, runTime, priority, mem);
            // ProcessFinished(processArray[(*size) - 1]);
        }
        (*size)++;
    }
    fclose(ptr);
    (*size)--;
    return processArray;
}

struct ScheduleType getChosenScheduling()
{
    struct ScheduleType scheduleType;
    int chosen_algorithm = 0;
    while (!(chosen_algorithm == 1 || chosen_algorithm == 2 || chosen_algorithm == 3))
    {
        printf("choose the scheduling algorithm\n");
        printf("1-Non-preemptive Highest Priority First (HPF)\n");
        printf("2-Shortest Remaining time Next (SRTN)\n");
        printf("3-Round Robin (RR)\n");
        scanf("%d", &chosen_algorithm);
    }
    scheduleType.type = chosen_algorithm;
    if (chosen_algorithm == 3)
    {
        int quantum = -1;
        while (quantum <= 0)
        {
            printf("enter the chosen quantum\n");
            scanf("%d", &quantum);
        }
        scheduleType.parameter = quantum;
    }
    return scheduleType;
}

char **split(char *string, char *seperators, int *count)
{
    // get the length of the string
    int len = strlen(string);

    // use count to keep a count of the number of substrings
    *count = 0;

    // We make one pass of the string to first determine how many substrings
    // we'll need to create, so we can allocate space for a large enough array
    // of pointer to strings.  The variable i will keep track of our current
    // index in the string
    int i = 0;
    while (i < len)
    {
        // skip over the next group of separator characters
        while (i < len)
        {
            // keep incrementing i until the character at index i is NOT found in the
            // separators array, indicating we've reached the next substring to create
            if (strchr(seperators, string[i]) == NULL)
                break;
            i++;
        }

        // skip over the next group of substring (i.e. non-separator characters),
        // we'll use old_i to verify that we actually did detect non-separator
        // characters (perhaps we're at the end of the string)
        int old_i = i;
        while (i < len)
        {
            // increment i until the character at index i IS found in the separators
            // array, indicating we've reached the next group of separator
            // character(s)
            if (strchr(seperators, string[i]) != NULL)
                break;
            i++;
        }

        // if we did encounter non-seperator characters, increase the count of
        // substrings that will need to be created
        if (i > old_i)
            *count = *count + 1;
    }

    // allocate space for a dynamically allocated array of *count* number of
    // pointers to strings
    char **strings = malloc(sizeof(char *) * *count);

    // we'll make another pass of the string using more or less the same logic as
    // above, but this time we'll dynamically allocate space for each substring
    // and store the substring into this space
    i = 0;

    // buffer will temporarily store each substring, string_index will keep track
    // of the current index we are storing the next substring into using the
    // dynamically allocated array above
    char buffer[16384];
    int string_index = 0;
    while (i < len)
    {
        // skip through the next group of separators, exactly the same as above
        while (i < len)
        {
            if (strchr(seperators, string[i]) == NULL)
                break;
            i++;
        }

        // store the next substring into the buffer char array, use j to keep
        // track of the index in the buffer array to store the next char
        int j = 0;
        while (i < len)
        {
            if (strchr(seperators, string[i]) != NULL)
                break;
            buffer[j] = string[i];
            i++;
            j++;
        }

        // only copy the substring into the array of substrings if we actually
        // read in characters with the above loop... it's possible we won't if
        // the string ends with a group of separator characters!
        if (j > 0)
        {
            // add a null terminator on to the end of buffer to terminate the string
            buffer[j] = '\0';

            // calculate how much space to allocate... we need to be able to store
            // the length of buffer (including a null terminator) number of characters
            int to_allocate = sizeof(char) *
                              (strlen(buffer) + 1);

            // allocate enough space using malloc, store the pointer into the strings
            // array of pointers at hte current string_index
            strings[string_index] = malloc(to_allocate);

            // copy the buffer into this dynamically allocated space
            strcpy(strings[string_index], buffer);

            // advance string_index so we store the next string at the next index in
            // the strings array
            string_index++;
        }
    }

    // return our array of strings
    return strings;
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

bool sendProcess(Process *process)
{
    if (msg_Id == -1)
    {
        return false;
    }
    msgsnd(msg_Id, (void *)process, sizeof(Process), IPC_NOWAIT);

    return true;
}

bool updateCounter(int counter)
{
    int *counterAddress = (int *)shmat(shm_Id, (void *)0, 0);
    if ((long)counterAddress == -1)
    {
        printf("Error in attaching the shm of the counter");
        return false;
    }
    *counterAddress = counter;
    return true;
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    msgctl(msg_Id, IPC_RMID, (struct msqid_ds *)0);
    shmctl(shm_Id, IPC_RMID, NULL);
    destroyClk(true);
    printf("process generator terminating!");
    killpg(getpgrp(), SIGINT);
    kill(getpid(), SIGKILL);
}