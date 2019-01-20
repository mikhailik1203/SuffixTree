#include "stdafx.h"
#include "SuffixTree.h"
#include "ContBuilder.h"
#include "StaticSuffixTree.h"
#include "StaticContBuilder.h"
//#include "MemAllocHook.h"
#include "hpUtils.h"
#include <cassert>
#include <iostream>
#include <map>
#include <iomanip>
#include <string>

namespace{
#ifdef _DEBUG
    const size_t LEVEL_1_KEYS_COUNT = 26;
    const size_t LEVEL_2_KEYS_COUNT = 26;
    const size_t LEVEL_3_KEYS_COUNT = 26;
    const size_t LEVEL_4_KEYS_COUNT = 26;
#else
    const size_t LEVEL_1_KEYS_COUNT = 16;//30;//56;//30
    const size_t LEVEL_2_KEYS_COUNT = 16;//32;//56;//32
    const size_t LEVEL_3_KEYS_COUNT = 20;//32;//56;//32
    const size_t LEVEL_4_KEYS_COUNT = 20;//33;//57;//33
#endif
    const char KEY_START_SYMBOL = 'A';

    const size_t BATCH_COUNT = 8;

    typedef std::vector<std::string> GeneratedKeyT;
    typedef std::vector<double> DurationsT;

    struct ComplexKey{
        size_t idx1_;
        size_t idx2_;
        size_t idx3_;
        size_t idx4_;
    };

    bool operator < (const ComplexKey &l, const ComplexKey &v)
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

    struct LatencyData{
        double min_;
        double max_;
        DurationsT latencies_;

        explicit LatencyData(size_t count):
            min_(1000000000), max_(0), latencies_(static_cast<size_t>(double(count)/BATCH_COUNT + 1), 0.0)
        {}
    };

    Key2IdxT prepareLevel1Keys()
    {
        Key2IdxT res;
        char value[] = "AAA";
        for(size_t i = 0; i < LEVEL_1_KEYS_COUNT; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    Key2IdxT prepareLevel2Keys()
    {
        Key2IdxT res;
        char value[] = "BBA";
        for(size_t i = 0; i < LEVEL_2_KEYS_COUNT; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    Key2IdxT prepareLevel3Keys()
    {
        Key2IdxT res;
        char value[] = "CCA";
        for(size_t i = 0; i < LEVEL_3_KEYS_COUNT; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    Key2IdxT prepareLevel4Keys()
    {
        Key2IdxT res;
        char value[] = "DDA";
        for(size_t i = 0; i < LEVEL_4_KEYS_COUNT; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    GeneratedKeyT generateKeys()
    {
        GeneratedKeyT res;
        res.reserve(LEVEL_1_KEYS_COUNT*LEVEL_2_KEYS_COUNT*LEVEL_3_KEYS_COUNT*LEVEL_4_KEYS_COUNT);
        char value[] = "AAA-BBA-CCA-DDA";
        int count = 1;
        for(size_t i = 0; i < LEVEL_1_KEYS_COUNT; ++i){
            for(size_t j = 0; j < LEVEL_2_KEYS_COUNT; ++j){
                for(size_t k = 0; k < LEVEL_3_KEYS_COUNT; ++k){
                    for(size_t l = 0; l < LEVEL_4_KEYS_COUNT; ++l){
                        res.push_back(value);
                        value[14] += 1;
                        ++count;
                    }
                    value[10] += 1;
                    value[14] = KEY_START_SYMBOL;
                }
                value[6] += 1;
                value[10] = KEY_START_SYMBOL;
                value[14] = KEY_START_SYMBOL;
            }
            value[2] += 1;
            value[6] = KEY_START_SYMBOL;
            value[10] = KEY_START_SYMBOL;
            value[14] = KEY_START_SYMBOL;
        }
        return res;
    }

    template<typename ContT, typename KeyT, typename ValueT>
    LatencyData latencyTest(
                  ContT &cont, 
                  const GeneratedKeyT &keys,
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

    struct LatencyBuckets{
        double startInterval_;
        double intervalLen_;
        std::vector<size_t> quantities_;
        size_t count_;

        LatencyBuckets(double minVal, double maxVal, size_t count):
            count_(0)
        {
            startInterval_ = minVal - 1;
            intervalLen_ = (maxVal - startInterval_)/count + 1;
            quantities_.assign(count + 1, 0);
        }
    };

    LatencyBuckets aggregateLatencyResults(const LatencyData &vals, size_t bucketsCount)
    {
        LatencyBuckets res(vals.min_, vals.max_, bucketsCount);

        std::for_each(std::begin(vals.latencies_), std::end(vals.latencies_), 
            [&res](double v)
            {
                if(fabs(v) <= std::numeric_limits<double>::epsilon())
                    return;
                double distance = v - res.startInterval_;
                size_t index = static_cast<size_t>(distance/res.intervalLen_);
                res.quantities_[index] += 1;
                ++res.count_;
            });

        return res;
    }

    LatencyBuckets aggregateLatencyResults(LatencyData vals, size_t bucketsCount, double perc)
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

        LatencyBuckets percBuckets(startInterval, lastPercInterval, bucketsCount);
        for(size_t i = startIdx; i < startIdx + countPerc; ++i)
        {
            double distance = vals.latencies_[i] - percBuckets.startInterval_;
            size_t index = static_cast<size_t>(distance/percBuckets.intervalLen_);
            percBuckets.quantities_[index] += 1;
            ++percBuckets.count_;
        }
        return percBuckets;
    }

    void output(const std::string &name, const LatencyBuckets &data)
    {
        std::cout << "Latency for " << name << ", count = " << data.count_*BATCH_COUNT<< ":"<< std::endl;
        double startInterval = data.startInterval_;
        double count = 0;
        std::for_each(std::begin(data.quantities_), std::end(data.quantities_), 
            [&](size_t v)
            {
                double endInterval = startInterval + data.intervalLen_;
                count += v;
                std::cout << "\t" << startInterval << ";"<< endInterval << ";" << v << ";" << count/data.count_*100<< std::endl;
                startInterval = endInterval;
            });
    }
}

namespace tst{


    void hpTimerGranularityTest()
    {
        hptimer::HighPerfTimer timer;
        std::cout << "Granulatiry of the HighPerfTimer is [" << timer.granularity() 
                << "]" << hptimer::HighPerfTimer::units() << std::endl;
    }

    LatencyData latencySuffixTreeTest()
    {
        GeneratedKeyT keys = generateKeys();
        std::random_shuffle(std::begin(keys), std::end(keys));

        ContBuilder builder;
        typedef suffix_tree::SuffixTree<ContBuilder, std::string, int> ContT;
        ContT cont(builder);
        std::cout<< "Latency of SuffixTree(adhock keys): "<< std::endl;

        LatencyData res = latencyTest<ContT, std::string, int>(
            cont, keys, BATCH_COUNT, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                auto it = cont.insert(key, value);
            }, 
            [](ContT &cont){},
            "inserts into");

        std::cout<< "\t minLatency[" << res.min_<< "], maxLatency[" << res.max_<< "]"<< std::endl;
        return res;
    }

    LatencyData latencyStlMap_byString_Test()
    {
        GeneratedKeyT keys = generateKeys();
        std::random_shuffle(std::begin(keys), std::end(keys));

        ContBuilder builder;
        typedef std::map<std::string, int> ContT;
        ContT cont;
        std::cout<< "Latency of std::map(string): "<< std::endl;

        LatencyData res = latencyTest<ContT, std::string, int>(
            cont, keys, BATCH_COUNT, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                cont[key] = value;
            }, 
            [](ContT &cont){},
            "inserts into");

        std::cout<< "\t minLatency[" << res.min_<< "], maxLatency[" << res.max_<< "]"<< std::endl;
        return res;
    }

    void performanceTestSuite()
    {
        hptimer::setProcessAffinity(3);
        hpTimerGranularityTest();
        LatencyData suffTreeLatency = latencySuffixTreeTest();
        LatencyData stlMapLatency = latencyStlMap_byString_Test();

        LatencyBuckets suffTreeBuckets = aggregateLatencyResults(suffTreeLatency, 1000);
        output("SuffixTree", suffTreeBuckets);
        LatencyBuckets suffTreeBuckets99Perc = aggregateLatencyResults(suffTreeLatency, 1000, 99);
        output("SuffixTree 99% details", suffTreeBuckets99Perc);

        LatencyBuckets stdMapBuckets = aggregateLatencyResults(stlMapLatency, 1000);
        output("StdMap", stdMapBuckets);
        LatencyBuckets stdMapBuckets99Perc = aggregateLatencyResults(stlMapLatency, 1000, 99);
        output("StdMap 99% details", stdMapBuckets99Perc);

    }

}