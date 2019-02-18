#include "SuffixTree.h"
#include "ContBuilder.h"
#include "StaticSuffixTree.h"
#include "StaticContBuilder.h"
#include "hpUtils.h"
#include "testUtils.h"
#include <cassert>
#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <string>

namespace tst{


    void hpTimerGranularityTest()
    {
        hptimer::HighPerfTimer timer;
        std::cout << "Granulatiry of the HighPerfTimer is [" << timer.granularity() 
                << "]" << hptimer::HighPerfTimer::units() << std::endl;
    }

    LatencyData insertLatencySuffixTreeTest(const KeyValues &vals)
    {
        ContBuilder builder;
        typedef suffix_tree::SuffixTree<ContBuilder, std::string, int> ContT;
        ContT cont(builder);
        std::cout<< "Latency of Insert at SuffixTree(adhock keys): "<< std::endl;

        LatencyData res = latencyTest<ContT, std::string, int>(
            cont, vals.genKeys_,
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                auto it = cont.insert(key, value);
            }, 
            [](ContT &cont){},
            "inserts into");

        showStatistics(res);
        return res;
    }

    LatencyData searchLatencySuffixTreeTest(const KeyValues &vals)
    {
        ContBuilder builder;
        typedef suffix_tree::SuffixTree<ContBuilder, std::string, int> ContT;
        ContT cont(builder);
        std::cout<< "Latency of Search at SuffixTree(adhock keys): "<< std::endl;

        size_t value = 1;
        for(auto &v: vals.genKeys_){
            cont.insert(v, ++value);
        }

        LatencyData res = latencyTest<ContT, std::string, int>(
                cont, vals.genKeys_,
                [](ContT &cont, const std::string &key, const int &value)->void
                {
                    auto it = cont.find(key.c_str());
                    assert(cont.end() != it);
                },
                [](ContT &cont){},
                "inserts into");

        showStatistics(res);
        return res;
    }

    LatencyData insertLatencyStlMap_byString_Test(const KeyValues &vals)
    {
        ContBuilder builder;
        typedef std::map<std::string, int> ContT;
        ContT cont;
        std::cout<< "Latency of insert at std::map(string): "<< std::endl;

        LatencyData res = latencyTest<ContT, std::string, int>(
            cont, vals.genKeys_,
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                cont[key] = value;
            }, 
            [](ContT &cont){},
            "inserts into");

        showStatistics(res);
        return res;
    }

    LatencyData searchLatencyStlMap_byString_Test(const KeyValues &vals)
    {
        ContBuilder builder;
        typedef std::map<std::string, int> ContT;
        ContT cont;
        std::cout<< "Latency of search at std::map(string): "<< std::endl;

        size_t value = 1;
        for(auto &v: vals.genKeys_){
            cont[v] = ++value;
        }

        LatencyData res = latencyTest<ContT, std::string, int>(
                cont, vals.genKeys_,
                [](ContT &cont, const std::string &key, const int &value)->void
                {
                    auto it = cont.find(key.c_str());
                    assert(cont.end() != it);
                },
                [](ContT &cont){},
                "inserts into");

        showStatistics(res);
        return res;
    }

    LatencyData insertLatencyStlHashMap_byString_Test(const KeyValues &vals)
    {
        ContBuilder builder;
        typedef std::unordered_map<std::string, int> ContT;
        ContT cont;
        std::cout<< "Latency of insert at std::unordered_map(string): "<< std::endl;

        LatencyData res = latencyTest<ContT, std::string, int>(
                cont, vals.genKeys_,
                [](ContT &cont, const std::string &key, const int &value)->void
                {
                    cont[key] = value;
                },
                [](ContT &cont){},
                "inserts into");

        showStatistics(res);
        return res;
    }

    LatencyData searchLatencyStlHashMap_byString_Test(const KeyValues &vals)
    {
        ContBuilder builder;
        typedef std::unordered_map<std::string, int> ContT;
        ContT cont;
        std::cout<< "Latency of search at std::unordered_map(string): "<< std::endl;

        size_t value = 1;
        for(auto &v: vals.genKeys_){
            cont[v] = ++value;
        }

        LatencyData res = latencyTest<ContT, std::string, int>(
                cont, vals.genKeys_,
                [](ContT &cont, const std::string &key, const int &value)->void
                {
                    auto it = cont.find(key.c_str());
                    assert(cont.end() != it);
                },
                [](ContT &cont){},
                "inserts into");

        showStatistics(res);
        return res;
    }

    void performanceTestSuite()
    {
        hptimer::setProcessAffinity(3);
        hpTimerGranularityTest();

        std::vector<size_t> keySizes = {3, 10, 20, 40};

#ifdef DEBUG
        std::vector<KeysCount> keysCounts = {{16, 16, 20, 20}};
#else
        std::vector<KeysCount> keysCounts = {{10, 10, 10, 10}, {16, 16, 20, 20}, {30, 32, 32, 33}, {56, 56, 56, 57}};
#endif

        StatisticsT stats;
        stats.reserve(keySizes.size()*keysCounts.size()*7);
        for(auto keySize: keySizes)
        {
            for(auto keysCount: keysCounts) {
                KeyValues vals;
                vals.subKeyLength_ = keySize;
                vals.keysCount_ =
                        keysCount.level1Count_*keysCount.level2Count_*
                        keysCount.level3Count_*keysCount.level4Count_;

                vals.lvl1Key_ = prepareLevel1Keys(vals.subKeyLength_, keysCount.level1Count_);
                vals.lvl2Key_ = prepareLevel2Keys(vals.subKeyLength_, keysCount.level2Count_);
                vals.lvl3Key_ = prepareLevel3Keys(vals.subKeyLength_, keysCount.level3Count_);
                vals.lvl4Key_ = prepareLevel4Keys(vals.subKeyLength_, keysCount.level4Count_);
                vals.genKeys_ = generateKeys(vals);
                std::random_shuffle(std::begin(vals.genKeys_), std::end(vals.genKeys_));
                std::cout << "----------------------- Latency of Insert for SubKeySize = " << vals.subKeyLength_
                          << " , KeysCount = " << vals.keysCount_
                          << " --------------------------------" << std::endl;

                {
                    StatisticData stat;

                    LatencyData suffTreeLatency = insertLatencySuffixTreeTest(vals);
                    LatencyData stlMapLatency = insertLatencyStlMap_byString_Test(vals);
                    LatencyData stlHashMapLatency = insertLatencyStlHashMap_byString_Test(vals);

                    LatencyBuckets aggrBuckets = aggregateLatencyResults(suffTreeLatency, stlMapLatency, stlHashMapLatency, 1000);
                    output("SuffixTree, StdMap, HashMap", 3, aggrBuckets);

                    LatencyBuckets aggrBuckets95Perc = aggregateLatencyResults(suffTreeLatency, stlMapLatency, stlHashMapLatency, 1000, 95);
                    output("SuffixTree95%, StdMap95%, HashMap95%", 3, aggrBuckets95Perc);
                }

                std::cout << "----------------------- Latency of Search for SubKeySize = " << vals.subKeyLength_
                          << " , KeysCount = " << vals.keysCount_
                          << " --------------------------------" << std::endl;

                std::random_shuffle(std::begin(vals.genKeys_), std::end(vals.genKeys_));
                {
                    StatisticData stat;

                    LatencyData suffTreeLatency = searchLatencySuffixTreeTest(vals);
                    LatencyData stlMapLatency = searchLatencyStlMap_byString_Test(vals);
                    LatencyData stlHashMapLatency = searchLatencyStlHashMap_byString_Test(vals);

                    LatencyBuckets aggrBuckets = aggregateLatencyResults(suffTreeLatency, stlMapLatency, stlHashMapLatency, 1000);
                    output("SuffixTree, StdMap, HashMap", 3, aggrBuckets);

                    LatencyBuckets aggrBuckets95Perc = aggregateLatencyResults(suffTreeLatency, stlMapLatency, stlHashMapLatency, 1000, 95);
                    output("SuffixTree95%, StdMap95%, HashMap95%", 3, aggrBuckets95Perc);
                }
            }
        }

    }

}