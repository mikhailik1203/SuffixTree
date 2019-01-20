#pragma once

#include <string>

namespace hptimer{
    class HighPerfTimer{
    public:
        HighPerfTimer();
        void start();
        void stop();
        double interval()const;

        static const std::string &units(){return UNITS;}
        double granularity()const;
    private:
        static std::string UNITS;
        LARGE_INTEGER startTime_, finishTime_;
        LARGE_INTEGER timerFreq_;
    };

    bool setProcessAffinity(char cpuId);
}