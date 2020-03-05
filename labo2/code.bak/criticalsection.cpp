#include "criticalsection.h"
#include <atomic>
#include <thread>
#include <algorithm>

void WonderfulCriticalSection::initialize(unsigned int nbThreads)
{
    tickets.resize(nbThreads, 0);
    choosing.resize(nbThreads, false);
}

void WonderfulCriticalSection::lock(unsigned int index)
{
    choosing[index] = true;
    tickets[index] = 1 + *std::max_element(tickets.begin(), tickets.end());
    choosing[index] = false;
    std::atomic_thread_fence(std::memory_order_acq_rel);

    for (unsigned int other = 0; other < choosing.size(); ++other) {
        while (choosing[other]) {
            std::this_thread::yield();
        }

        while(tickets[other] != 0 && (tickets[other] < tickets[index] ||
                                        (tickets[other] == tickets[index] && other < index))) {
            std::this_thread::yield();
        }
    }
}

void WonderfulCriticalSection::unlock(unsigned int thread)
{
    tickets[thread] = 0;
}
