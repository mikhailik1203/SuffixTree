#include "hpUtils.h"
#include "ContBuilder.h"
#include "MemAllocHook.h"
#include <cassert>
#include <iostream>
#include <map>
#include <iomanip>
#include <string>
#include <thread>
#include <vector>

namespace tst{
    struct KeysCount{
        size_t level1Count_;
        size_t level2Count_;
        size_t level3Count_;
        size_t level4Count_;
    };
    const char KEY_START_SYMBOL = 'A';
    const char KEY_DELIMETER = '-';
    const size_t BATCH_COUNT = 4;

    typedef std::vector<std::string> GeneratedKeyT;
    typedef std::vector<double> DurationsT;

    struct KeyValues{
        aux::Key2IdxT lvl1Key_, lvl2Key_, lvl3Key_, lvl4Key_;
        GeneratedKeyT genKeys_;
        size_t subKeyLength_;
        size_t keysCount_;
    };

    struct StatValues{
        size_t memUsage_;
        double duration_;

        StatValues(): memUsage_(0), duration_(0.0)
        {}
    };

    struct StatisticData{
        std::string name_;
        StatValues initStat_;
        StatValues insertStat_;
        StatValues searchStat_;
    };
    typedef std::vector<StatisticData> StatisticsT;

    struct ComplexKey{
        size_t idx1_;
        size_t idx2_;
        size_t idx3_;
        size_t idx4_;
    };
    bool operator < (const ComplexKey &l, const ComplexKey &v);
    bool operator == (const ComplexKey &l, const ComplexKey &v);


    aux::Key2IdxT prepareLevel1Keys(size_t keyLength, size_t keyCount);
    aux::Key2IdxT prepareLevel2Keys(size_t keyLength, size_t keyCount);
    aux::Key2IdxT prepareLevel3Keys(size_t keyLength, size_t keyCount);
    aux::Key2IdxT prepareLevel4Keys(size_t keyLength, size_t keyCount);

    GeneratedKeyT generateKeys(const KeyValues &keyVals, char delim = KEY_DELIMETER);

    template<typename ContT, typename KeyT, typename ValueT>
    StatValues perfTest(
                  const KeyValues &keyVals,
                  ContT &cont,
                  std::function<void(ContT &cont, const KeyT &, const ValueT &val)> insertAndCheckFunc, 
                  std::function<void(ContT &cont)> postProcFunc)
    {
        StatValues stat;
        const GeneratedKeyT &keys = keyVals.genKeys_;

        hptimer::HighPerfTimer timer;
        volatile size_t memUsageBefore = mem_alloc::processMemUsage();
        timer.start();
        for(size_t i = 0; i < keys.size(); ++i){
            insertAndCheckFunc(cont, keys[i], static_cast<int>(i + 1));
        }
        postProcFunc(cont);
        timer.stop();
        volatile size_t memUsageAfter = mem_alloc::processMemUsage();
        stat.memUsage_ = memUsageAfter - memUsageBefore;
        stat.duration_ = timer.interval();
        return stat;
    }

    struct LatencyData{
        double min_;
        double max_;
        DurationsT latencies_;

        explicit LatencyData(size_t count):
                min_(1000000000), max_(0), latencies_(count, 0.0)
        {}
    };

    template<typename ContT, typename KeyT, typename ValueT>
    LatencyData latencyTest(
            ContT &cont,
            const tst::GeneratedKeyT &keys,
            size_t batchSize,
            std::function<void(ContT &, const KeyT &, const ValueT &)> insertAndCheckFunc,
            std::function<void(ContT &)> postProcFunc,
            const std::string &contName)
    {
        size_t count = keys.size();
        LatencyData res(count);
        hptimer::HighPerfTimer timer;
        size_t i = 0;
        size_t bucketIdx = 0;
        while(i < count){
            timer.start();
            for(size_t j = 0; j < batchSize; ++j, ++i){
                insertAndCheckFunc(cont, keys[i], static_cast<int>(count));
            }
            timer.stop();
            double v = timer.interval();
            res.latencies_[bucketIdx] = v;
            res.min_ = std::min(res.min_, v);
            res.max_ = std::max(res.max_, v);
            ++bucketIdx;
        }
        return res;
    }

    template<typename ContT, typename KeyT, typename ValueT>
    LatencyData latencyTest(
            ContT &cont,
            const tst::GeneratedKeyT &keys,
            std::function<void(ContT &, const KeyT &, const ValueT &)> insertAndCheckFunc,
            std::function<void(ContT &)> postProcFunc,
            const std::string &contName)
    {
        size_t count = keys.size();
        LatencyData res(count);
        hptimer::HighPerfTimer timer;
        size_t i = 0;
        while(i < count){
            timer.start();
            insertAndCheckFunc(cont, keys[i], static_cast<int>(count));
            timer.stop();
            double v = timer.interval();
            res.latencies_[i] = v;
            res.min_ = std::min(res.min_, v);
            res.max_ = std::max(res.max_, v);
            ++i;
        }
        return res;
    }

    struct LatencyBuckets{
        double startInterval_;
        double intervalLen_;
        std::vector<size_t> quantities_;
        size_t count_;
        size_t measurementsCount_;
        size_t bucketsCount_;

        LatencyBuckets(size_t measurmtCount, double minVal, double maxVal, size_t count):
                count_(0), bucketsCount_(count), measurementsCount_(measurmtCount)
        {
            startInterval_ = minVal - 1;
            intervalLen_ = (maxVal - startInterval_)/count + 1;
            quantities_.assign((count + 1)*measurementsCount_, 0);
        }
    };

    LatencyBuckets aggregateLatencyResults(const LatencyData &vals, size_t bucketsCount);
    LatencyBuckets aggregateLatencyResults(const LatencyData &vals, const LatencyData &vals2, size_t bucketsCount);
    LatencyBuckets aggregateLatencyResults(const LatencyData &vals, const LatencyData &vals2, const LatencyData &vals3, size_t bucketsCount);

    LatencyBuckets aggregateLatencyResults(LatencyData vals, size_t bucketsCount, double perc);
    LatencyBuckets aggregateLatencyResults(LatencyData vals1, LatencyData vals2, size_t bucketsCount, double perc);
    LatencyBuckets aggregateLatencyResults(LatencyData vals1, LatencyData vals2, LatencyData vals3, size_t bucketsCount, double perc);

    void output(const std::string &names, size_t measurementCount, const LatencyBuckets &data);
    void showStatistics(LatencyData &vals);
}

namespace std {

    template <>
    struct hash<tst::ComplexKey>
    {
        std::size_t operator()(const tst::ComplexKey& k) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return ((((hash<size_t>()(k.idx1_)^
                      (hash<size_t>()(k.idx2_) << 1)) >> 1)^
                     (hash<size_t>()(k.idx3_) << 1)) >> 1)^
                    (hash<size_t>()(k.idx4_) << 1);
        }
    };

}

