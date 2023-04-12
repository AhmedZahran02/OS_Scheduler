#include "headers.h"

void handler(int signum);

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGUSR1, handler);

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    // habd zone
    while (1)
    {
        // donot kill my clock plz
    }
    // end of habd zone

    destroyClk(true);
}
