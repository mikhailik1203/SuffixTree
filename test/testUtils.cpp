#include "testUtils.h"
#include "hpUtils.h"
#include "MemAllocHook.h"
#include <cassert>
#include <iostream>
#include <map>
#include <iomanip>
#include <string>
#include <thread>
#include <cmath>

using namespace tst;

namespace{
    aux::Key2IdxT prepareLevelKeys(char keyBase, char keyStart, size_t keyLength, size_t keyCount)
    {
        aux::Key2IdxT res;

        std::string value(keyLength, keyBase);
        value[keyLength - 1] = keyStart;
        for(size_t i = 0; i < keyCount; ++i){
            res.push_back(value);
            value[keyLength - 1] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    struct LatencyParams{
        double startInterval_ = 0.0;
        double lastPercInterval_ = 0.0;
        size_t startIdx_ = 0;
        size_t countPerc_ = 0;
    };

    LatencyParams calcLatencyParams(LatencyData &vals, double perc)
    {
        LatencyParams res;
        std::sort(std::begin(vals.latencies_), std::end(vals.latencies_));
        auto it = std::find_if(std::begin(vals.latencies_), std::end(vals.latencies_),
                               [](double v)
                               {
                                   return fabs(v) > std::numeric_limits<double>::epsilon();
                               });
        res.startIdx_ = it - std::begin(vals.latencies_);
        size_t count = vals.latencies_.size() - res.startIdx_;
        res.countPerc_ = static_cast<size_t>(double(count)*perc/100 + 1);

        res.startInterval_ = *it;
        res.lastPercInterval_ = vals.latencies_[res.startIdx_ + res.countPerc_];
        return res;
    }

    void fillLatencyMeasurement(LatencyData &vals, LatencyParams &params, LatencyBuckets &buckets, size_t measuremtIdx)
    {
        for(size_t i = params.startIdx_; i < params.startIdx_ + params.countPerc_; ++i)
        {
            double distance = vals.latencies_[i] - buckets.startInterval_;
            size_t index = buckets.bucketsCount_*measuremtIdx +
                           static_cast<size_t>(distance/buckets.intervalLen_);
            ++buckets.quantities_[index];
        }
        buckets.count_ = vals.latencies_.size();
    }

}

bool tst::operator < (const ComplexKey &l, const ComplexKey &v)
{
    if(l.idx1_ == v.idx1_){
        if(l.idx2_ == v.idx2_){
            if(l.idx3_ == v.idx3_){
                return l.idx4_ < v.idx4_;
            }else
                return l.idx3_ < v.idx3_;
        }else
            return l.idx2_ < v.idx2_;
    }else
        return l.idx1_ < v.idx1_;
}

bool tst::operator == (const ComplexKey &l, const ComplexKey &v)
{
    return (l.idx1_ == v.idx1_) &&
           (l.idx2_ == v.idx2_) &&
           (l.idx3_ == v.idx3_) &&
           (l.idx4_ == v.idx4_);
}

aux::Key2IdxT tst::prepareLevel1Keys(size_t keyLength, size_t keyCount)
{
    return prepareLevelKeys(KEY_START_SYMBOL, KEY_START_SYMBOL, keyLength, keyCount);
}

aux::Key2IdxT tst::prepareLevel2Keys(size_t keyLength, size_t keyCount)
{
    return prepareLevelKeys(KEY_START_SYMBOL + 1, KEY_START_SYMBOL, keyLength, keyCount);
}

aux::Key2IdxT tst::prepareLevel3Keys(size_t keyLength, size_t keyCount)
{
    return prepareLevelKeys(KEY_START_SYMBOL + 2, KEY_START_SYMBOL, keyLength, keyCount);
}

aux::Key2IdxT tst::prepareLevel4Keys(size_t keyLength, size_t keyCount)
{
    return prepareLevelKeys(KEY_START_SYMBOL + 3, KEY_START_SYMBOL, keyLength, keyCount);
}

GeneratedKeyT tst::generateKeys(const KeyValues &keyVals, char delim)
{
    GeneratedKeyT res;
    res.reserve(keyVals.keysCount_);
    for(auto &lvl1: keyVals.lvl1Key_){
        for(auto &lvl2: keyVals.lvl2Key_){
            for(auto &lvl3: keyVals.lvl3Key_){
                for(auto &lvl4: keyVals.lvl4Key_){
                    std::string key = lvl1 + delim + lvl2 + delim + lvl3 + delim + lvl4;
                    res.push_back(key);
                }
            }
        }
    }
    return res;
}


LatencyBuckets tst::aggregateLatencyResults(const LatencyData &vals, size_t bucketsCount)
{
    LatencyBuckets res(1, vals.min_, vals.max_, bucketsCount);

    std::for_each(std::begin(vals.latencies_), std::end(vals.latencies_),
                  [&res](double v)
                  {
                      if(std::fabs(v) <= std::numeric_limits<double>::epsilon())
                          return;
                      double distance = v - res.startInterval_;
                      size_t index = static_cast<size_t>(distance/res.intervalLen_);
                      ++res.quantities_[index];
                      ++res.count_;
                  });

    return res;
}

LatencyBuckets tst::aggregateLatencyResults(LatencyData vals, size_t bucketsCount, double perc)
{
    std::sort(std::begin(vals.latencies_), std::end(vals.latencies_));
    auto it = std::find_if(std::begin(vals.latencies_), std::end(vals.latencies_),
                           [](double v)
                           {
                               return fabs(v) > std::numeric_limits<double>::epsilon();
                           });
    size_t startIdx = it - std::begin(vals.latencies_);
    size_t count = vals.latencies_.size() - startIdx;
    size_t countPerc = static_cast<size_t>(double(count)*perc/100 + 1);

    double startInterval = *it;
    double lastPercInterval = vals.latencies_[startIdx + countPerc];

    LatencyBuckets percBuckets(1, startInterval, lastPercInterval, bucketsCount);
    for(size_t i = startIdx; i < startIdx + countPerc; ++i)
    {
        double distance = vals.latencies_[i] - percBuckets.startInterval_;
        size_t index = static_cast<size_t>(distance/percBuckets.intervalLen_);
        ++percBuckets.quantities_[index];
        ++percBuckets.count_;
    }
    return percBuckets;
}

LatencyBuckets tst::aggregateLatencyResults(LatencyData vals1, LatencyData vals2, size_t bucketsCount, double perc)
{
    LatencyParams latencyParam1 = calcLatencyParams(vals1, perc);
    LatencyParams latencyParam2 = calcLatencyParams(vals2, perc);

    LatencyBuckets percBuckets(
            2,
            std::min(latencyParam1.startInterval_, latencyParam2.startInterval_),
            std::max(latencyParam1.lastPercInterval_, latencyParam2.lastPercInterval_), bucketsCount);
    fillLatencyMeasurement(vals1, latencyParam1, percBuckets, 0);
    fillLatencyMeasurement(vals2, latencyParam2, percBuckets, 1);

    return percBuckets;
}

LatencyBuckets tst::aggregateLatencyResults(LatencyData vals1, LatencyData vals2, LatencyData vals3, size_t bucketsCount, double perc)
{
    LatencyParams latencyParam1 = calcLatencyParams(vals1, perc);
    LatencyParams latencyParam2 = calcLatencyParams(vals2, perc);
    LatencyParams latencyParam3 = calcLatencyParams(vals3, perc);

    LatencyBuckets percBuckets(
            3,
            std::min(std::min(latencyParam1.startInterval_, latencyParam2.startInterval_),
                    latencyParam3.startInterval_),
            std::max(std::max(latencyParam1.lastPercInterval_, latencyParam2.lastPercInterval_),
                    latencyParam3.lastPercInterval_),
            bucketsCount);

    fillLatencyMeasurement(vals1, latencyParam1, percBuckets, 0);
    fillLatencyMeasurement(vals2, latencyParam2, percBuckets, 1);
    fillLatencyMeasurement(vals3, latencyParam3, percBuckets, 2);
    return percBuckets;
}

void tst::output(const std::string &names, size_t measurementCount, const LatencyBuckets &data)
{
    std::cout << "Latency for " << names << ", count = " << data.count_*BATCH_COUNT<< ":"<< std::endl
            << "\tStartInterval;EndInterval;Quantity_X;Perc_X"<< std::endl;
    double startInterval = data.startInterval_;
    std::vector<double> counts(measurementCount, 0.0);
    for(size_t i = 0; i < data.bucketsCount_; ++i){
        double endInterval = startInterval + data.intervalLen_;
        std::cout << "\t" << startInterval << ";"<< endInterval << ";";

        for(size_t m = 0; m < measurementCount; ++m){
            size_t value = data.quantities_[i + data.bucketsCount_*m];
            counts[m] += value;

            std::cout << value << ";" << counts[m]/data.count_*100 << ";";
        }
        std::cout << std::endl;
        startInterval = endInterval;
    }
}


LatencyBuckets tst::aggregateLatencyResults(const LatencyData &vals, const LatencyData &vals2, size_t bucketsCount)
{
    LatencyBuckets res(
                2,
                std::min(vals.min_, vals2.min_),
                std::max(vals.max_, vals2.max_),
                bucketsCount);

    std::for_each(std::begin(vals.latencies_), std::end(vals.latencies_),
                  [&](double v)
                  {
                      if(std::fabs(v) <= std::numeric_limits<double>::epsilon())
                          return;
                      double distance = v - res.startInterval_;
                      size_t index = res.bucketsCount_*0 + static_cast<size_t>(distance/res.intervalLen_);
                      ++res.quantities_[index];
                  });

    std::for_each(std::begin(vals2.latencies_), std::end(vals2.latencies_),
                  [&](double v)
                  {
                      if(std::fabs(v) <= std::numeric_limits<double>::epsilon())
                          return;
                      double distance = v - res.startInterval_;
                      size_t index = res.bucketsCount_*1 + static_cast<size_t>(distance/res.intervalLen_);
                      ++res.quantities_[index];
                  });
    return res;
}

LatencyBuckets tst::aggregateLatencyResults(const LatencyData &vals, const LatencyData &vals2, const LatencyData &vals3, size_t bucketsCount)
{
    LatencyBuckets res(
            3,
            std::min(std::min(vals.min_, vals2.min_), vals3.min_),
            std::max(std::max(vals.max_, vals2.max_), vals3.max_),
            bucketsCount);

    std::for_each(std::begin(vals.latencies_), std::end(vals.latencies_),
                  [&](double v)
                  {
                      if(std::fabs(v) <= std::numeric_limits<double>::epsilon())
                          return;
                      double distance = v - res.startInterval_;
                      size_t index = res.bucketsCount_*0 + static_cast<size_t>(distance/res.intervalLen_);
                      ++res.quantities_[index];
                      ++res.count_;
                  });

    std::for_each(std::begin(vals2.latencies_), std::end(vals2.latencies_),
                  [&](double v)
                  {
                      if(std::fabs(v) <= std::numeric_limits<double>::epsilon())
                          return;
                      double distance = v - res.startInterval_;
                      size_t index = res.bucketsCount_*1 + static_cast<size_t>(distance/res.intervalLen_);
                      ++res.quantities_[index];
                  });
    std::for_each(std::begin(vals3.latencies_), std::end(vals3.latencies_),
                  [&](double v)
                  {
                      if(std::fabs(v) <= std::numeric_limits<double>::epsilon())
                          return;
                      double distance = v - res.startInterval_;
                      size_t index = res.bucketsCount_*2 + static_cast<size_t>(distance/res.intervalLen_);
                      ++res.quantities_[index];
                  });

    return res;
}

void tst::showStatistics(LatencyData &vals)
{
    std::sort(std::begin(vals.latencies_), std::end(vals.latencies_));
    auto it = std::find_if(std::begin(vals.latencies_), std::end(vals.latencies_),
                           [](double v)
                           {
                               return fabs(v) > std::numeric_limits<double>::epsilon();
                           });
    size_t startIdx = it - std::begin(vals.latencies_);
    size_t count = vals.latencies_.size() - startIdx;
    size_t count50Perc = static_cast<size_t>(double(count)*50/100 + 1);
    size_t count95Perc = static_cast<size_t>(double(count)*95/100 + 1);
    std::cout<< "\t minLatency[" << vals.latencies_[startIdx]<< "], maxLatency[" << vals.latencies_.back()<< "]"<< std::endl;
    std::cout<< "\t Latency50%[" << vals.latencies_[count50Perc]<< "], Latency95%[" << vals.latencies_[count95Perc]<< "]"<< std::endl;
}