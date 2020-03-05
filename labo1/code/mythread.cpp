#include "mythread.h"
#include <pcosynchro/pcomutex.h>

static volatile long unsigned int counter;
static PcoMutex mutex;

void runTask(unsigned long nbIterations)
{
    long unsigned int i = 0;

    while (i < nbIterations)
    {

        mutex.lock();
        counter++;
        mutex.unlock();

        i++;
    }
}

void initCounter()
{
    counter = 0;
}

long unsigned int getCounter()
{
    return counter;
}
