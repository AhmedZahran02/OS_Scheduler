#include "headers.h"

void handler(int signum);

#include "queue2.h"
int main(int argc, char *argv[])
{
    initClk();
    signal(SIGUSR1, handler);

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    // Queue of current arrived processes
    struct Queue2 Arrived_Processes ;









    // habd zone
    while (1)
    {
        // donot kill my clock plz
    }
    // end of habd zone

    destroyClk(true);
}
