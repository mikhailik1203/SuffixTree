#include "hpUtils.h"
#include <ratio>
#ifdef __linux__
#include <sched.h>
#endif

std::string hptimer::HighPerfTimer::UNITS = "nsec";

hptimer::HighPerfTimer::HighPerfTimer()
{
}

double hptimer::HighPerfTimer::granularity()const
{
    std::chrono::high_resolution_clock::time_point s = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point e = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count();
}

void hptimer::HighPerfTimer::start()
{
    startTime_ = std::chrono::high_resolution_clock::now();
}
void hptimer::HighPerfTimer::stop()
{
    finishTime_ = std::chrono::high_resolution_clock::now();
}

double hptimer::HighPerfTimer::interval()const
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finishTime_ - startTime_).count();
}

bool hptimer::setProcessAffinity(char cpuId)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpuId, &mask);
    int status = sched_setaffinity(0, sizeof(mask), &mask);
    return 0 == status;
}
