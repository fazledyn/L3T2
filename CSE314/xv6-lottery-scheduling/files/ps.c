#include "pstat.h"
#include "types.h"
#include "user.h"

#include "random.h"

int main(int argc, char* argv[])
{
    struct pstat pt;
    if (getpinfo(&pt) < 0)
    {
        printf(1, "ps: getpinfo(&pt) failed\n");
        exit();
    }

    int i;
    printf(1, "PID\tTickets\tTicks\n");
    for (i=0; i < NPROC; i++)
    {
        if (pt.inuse[i] && pt.pid[i] != 0)
        {
            printf(1, "%d\t%d\t%d\n", pt.pid[i], pt.tickets[i], pt.ticks[i]);
        }
    }

    exit();
}