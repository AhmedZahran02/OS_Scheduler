// #include "PriorityQueue2.h"
// #include "queue2.h"

// /* Modify this file as needed*/
// // int remainingtime;

// #define CONNKEY 400
// int msgq_id, rec_val;
// void *shmAddrCnt;

// struct PriorityQueue2 currProcesses;

// void handler(int signum);

// int main(int agrc, char *argv[])
// {
//     initClk();
//     signal(SIGUSR1, handler);

//     msgq_id = msgget(CONNKEY, 0666 | IPC_CREAT);
//     shmAddrCnt = shmat(CONNKEY, (void *)0, 0);
//     if (shmAddrCnt == -1)
//     {
//         perror("Error in attach in writer");
//         exit(-1);
//     }
//     // TODO it needs to get the remaining time from somewhere
//     // remainingtime = ??;
//     //  while (remainingtime > 0)
//     //  {
//     //      // remainingtime = ??;
//     //  }

//     while (1)
//     {
//         /*

//         there is a shared memory that contains the remaining time of running process and its id
//         this memory get updated in process.c
//         there is a variable contains the remaining time of the running process
//         you have array of structs of all processes
//         you have array of structs of current processes in the system

//         get the current clock
//         loop on all processes to push the ones that arrival time less than current time
//         loop on current processes to get the min running time one
//         if current running process equall null {
//             run the choosed pro
//         }



//         int clk = getclk();



//         loop on the array

//         get the min remaining time


//         */
//     }

//     // destroyClk(false);

//     return 0;
// }

// void handler(int signum)
// {
//     int cnt = (int *)shmAddrCnt;
//     for (int i = 0; i < cnt; i++)
//     {
//         struct Process tempProcess;
//         rec_val = msgrcv(msgq_id, &(tempProcess), sizeof(tempProcess), 0, !IPC_NOWAIT);
//         }
// }