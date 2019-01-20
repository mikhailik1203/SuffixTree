#include "stdafx.h"
#include "hpUtils.h"

std::string hptimer::HighPerfTimer::UNITS = "nsec";

hptimer::HighPerfTimer::HighPerfTimer()
{
    QueryPerformanceFrequency(&timerFreq_); 
}

double hptimer::HighPerfTimer::granularity()const
{
    return double(1000000000)/timerFreq_.QuadPart;
}

void hptimer::HighPerfTimer::start()
{
    QueryPerformanceCounter(&startTime_);
}
void hptimer::HighPerfTimer::stop()
{
    QueryPerformanceCounter(&finishTime_);
}

double hptimer::HighPerfTimer::interval()const
{
    return double(finishTime_.QuadPart - startTime_.QuadPart)*1000000000/timerFreq_.QuadPart;
}

bool hptimer::setProcessAffinity(char cpuId)
{
    HANDLE process = GetCurrentProcess();
    DWORD_PTR processAffinityMask = 1 << cpuId;
    return TRUE == SetProcessAffinityMask(process, processAffinityMask);    
}
