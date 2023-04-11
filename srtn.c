#include "headers.h"

/* Modify this file as needed*/
// int remainingtime;

#define CONNKEY 400
#define NUMKEY 500
int msgq_id, send_val;

int main(int agrc, char *argv[])
{
    initClk();

    msgq_id = msgget(CONNKEY, 0666 | IPC_CREAT);

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    //  while (remainingtime > 0)
    //  {
    //      // remainingtime = ??;
    //  }

    while (1)
    {
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
