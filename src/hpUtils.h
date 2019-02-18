#pragma once

#include <string>
#include <chrono>

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
        std::chrono::high_resolution_clock::time_point startTime_, finishTime_;
    };

    bool setProcessAffinity(char cpuId);
}