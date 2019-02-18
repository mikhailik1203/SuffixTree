#include "SuffixTree.h"
#include "ContBuilder.h"
#include "StaticSuffixTree.h"
#include "StaticContBuilder.h"
#include "hpUtils.h"
#include "MemAllocHook.h"
#include "testUtils.h"
#include <cassert>
#include <iostream>
#include <map>
#include <unordered_map>
#include <iomanip>
#include <string>
#include <thread>

namespace tst{

    StatisticData perfSuffixTreeTest(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "SuffixTree[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder(
                keyVals.lvl1Key_,
                keyVals.lvl2Key_,
                keyVals.lvl3Key_,
                keyVals.lvl4Key_);
        size_t memUsageBefore = mem_alloc::processMemUsage();
        typedef suffix_tree::SuffixTree<ContBuilder, std::string, int> ContT;
        ContT cont(builder);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ =
                perfTest<ContT, std::string, int>(
                    keyVals,
                    cont,
                    [](ContT &cont, const std::string &k, const int &value)->void
                    {
                        auto it = cont.insert(k.c_str(), value);

                        assert(value == cont.size());
                        assert(cont.end() != it);
                        assert(value == *it);
                    },
                    [](ContT &cont){});

        stat.searchStat_ =
                perfTest<ContT, std::string, int>(
                    keyVals,
                    cont,
                    [](ContT &cont, const std::string &k, const int &value)->void
                    {
                        auto it = cont.find(k.c_str());
                        assert(cont.end() != it);
                        assert(value == *it);
                    },
                    [](ContT &cont){});
        return stat;
    }

    StatisticData perfSuffixTree_adhockKeysTest(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "SuffixTree_adhockKeys[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder;
        size_t memUsageBefore = mem_alloc::processMemUsage();
        typedef suffix_tree::SuffixTree<ContBuilder, std::string, int> ContT;
        ContT cont(builder);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ =
                perfTest<ContT, std::string, int>(
                    keyVals,
                    cont,
                    [](ContT &cont, const std::string &k, const int &value)->void
                    {
                        auto it = cont.insert(k.c_str(), value);

                        assert(value == cont.size());
                        assert(cont.end() != it);
                        assert(value == *it);
                    },
                    [](ContT &cont){});

        stat.searchStat_ =
                perfTest<ContT, std::string, int>(
                    keyVals,
                    cont,
                    [](ContT &cont, const std::string& k, const int &value)->void
                    {
                        auto it = cont.find(k.c_str());
                        assert(cont.end() != it);
                        assert(value == *it);
                    },
                    [](ContT &cont){});
        return stat;
    }

    StatisticData perfStlMap_byString_Test(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "StlMap_byString[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        typedef std::map<std::string, int> ContT;
        ContT cont;
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [](ContT &cont, const std::string &k, const int &value)->void
                {
                    /// need to create new string, otherwise key will make a copy later
                    cont.insert(ContT::value_type(k.c_str(), value));

                    assert(value == cont.size());
                },
                [](ContT &cont){});

        stat.searchStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [](ContT &cont, const std::string &k, const int &value)->void
                {
                    auto it = cont.find(k.c_str());
                    assert(cont.end() != it);
                },
                [](ContT &cont){});
        return stat;
    }

    StatisticData perfStlMap_byStructKeyTest(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "StlMap_byStructKey[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder(
                keyVals.lvl1Key_,
                keyVals.lvl2Key_,
                keyVals.lvl3Key_,
                keyVals.lvl4Key_);
        typedef std::map<ComplexKey, int> ContT;
        ContT cont;
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [&builder](ContT &cont, const std::string &k, const int &value)->void
                {
                    ComplexKey complexKey;
                    ContBuilder::ParsedKeyT parsedKey;
                    if(!builder.parseKey(k.c_str(), parsedKey))
                        throw std::runtime_error("Unable to parse string key to indexes");
                    complexKey.idx1_ = parsedKey[0];
                    complexKey.idx2_ = parsedKey[1];
                    complexKey.idx3_ = parsedKey[2];
                    complexKey.idx4_ = parsedKey[3];
                    cont[complexKey] = value;

                    assert(value == cont.size());
                },
                [](ContT &cont){});

        stat.searchStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [&builder](ContT &cont, const std::string &k, const int &)->void
                {
                    ComplexKey complexKey;
                    ContBuilder::ParsedKeyT parsedKey;
                    if(!builder.parseKey(k.c_str(), parsedKey))
                        throw std::runtime_error("Unable to parse string key to indexes");
                    complexKey.idx1_ = parsedKey[0];
                    complexKey.idx2_ = parsedKey[1];
                    complexKey.idx3_ = parsedKey[2];
                    complexKey.idx4_ = parsedKey[3];

                    auto it = cont.find(complexKey);
                    assert(cont.end() != it);
                },
                [](ContT &cont){});
        return stat;
    }

    StatisticData perfSortedStlVector_byStringTest(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "SortedStlVector_byString[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();

        ContBuilder builder(
                keyVals.lvl1Key_,
                keyVals.lvl2Key_,
                keyVals.lvl3Key_,
                keyVals.lvl4Key_);
        typedef std::pair<std::string, int> ValT;
        typedef std::vector<ValT> ContT;
        ContT cont;
        cont.reserve(keyVals.keysCount_);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [&builder](ContT &cont, const std::string &k, const int &value)->void
                {
                    cont.push_back(ValT(k.c_str(), value));
                    assert(value == cont.size());
                },
                [](ContT &cont){
                    cont.shrink_to_fit();
                    std::sort(cont.begin(), cont.end(), [](const ValT &lft, const ValT &rght){ return lft.first < rght.first;});
                });

        stat.searchStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [&builder](ContT &cont, const std::string &k, const int &)->void
                {
                    auto it = std::lower_bound(cont.begin(), cont.end(), ValT(k.c_str(), 0),
                        [](const ValT &lft, const ValT &rght){return lft.first < rght.first;});
                    assert(cont.end() != it);
                    assert(k == it->first);
                },
                [](ContT &cont){});
        return stat;
    }

    StatisticData perfSortedStlVector_byStructTest(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "SortedStlVector_byStruct[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder(
                keyVals.lvl1Key_,
                keyVals.lvl2Key_,
                keyVals.lvl3Key_,
                keyVals.lvl4Key_);
        typedef std::pair<ComplexKey, int> ValT;
        typedef std::vector<ValT> ContT;
        ContT cont;
        cont.reserve(keyVals.keysCount_);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [&builder](ContT &cont, const std::string &k, const int &value)->void
                {
                    ComplexKey complexKey;
                    ContBuilder::ParsedKeyT parsedKey;
                    if(!builder.parseKey(k.c_str(), parsedKey))
                        throw std::runtime_error("Unable to parse string key to indexes");
                    complexKey.idx1_ = parsedKey[0];
                    complexKey.idx2_ = parsedKey[1];
                    complexKey.idx3_ = parsedKey[2];
                    complexKey.idx4_ = parsedKey[3];
                    cont.push_back(ValT(complexKey, value));

                    assert(value == cont.size());
                },
                [](ContT &cont)
                {
                    cont.shrink_to_fit();
                    std::sort(cont.begin(), cont.end(), [](const ValT &lft, const ValT &rght){ return lft.first < rght.first;});
                });

        stat.searchStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [&builder](ContT &cont, const std::string &k, const int &)->void
                {
                    ComplexKey complexKey;
                    ContBuilder::ParsedKeyT parsedKey;
                    if(!builder.parseKey(k.c_str(), parsedKey))
                        throw std::runtime_error("Unable to parse string key to indexes");
                    complexKey.idx1_ = parsedKey[0];
                    complexKey.idx2_ = parsedKey[1];
                    complexKey.idx3_ = parsedKey[2];
                    complexKey.idx4_ = parsedKey[3];

                    auto it = std::lower_bound(cont.begin(), cont.end(), ValT(complexKey, 0),
                        [](const ValT &lft, const ValT &rght){return lft.first < rght.first;});
                    assert(cont.end() != it);
                    assert(complexKey.idx1_ == it->first.idx1_);
                    assert(complexKey.idx2_ == it->first.idx2_);
                    assert(complexKey.idx3_ == it->first.idx3_);
                    assert(complexKey.idx4_ == it->first.idx4_);
                },
                [](ContT &cont){});
        return stat;
    }

    StatisticData perfStaticSuffixTreeTest(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "StaticSuffixTree[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        StaticContBuilder builder(
                keyVals.lvl1Key_,
                keyVals.lvl2Key_,
                keyVals.lvl3Key_,
                keyVals.lvl4Key_);
        typedef int ValueT;
        typedef st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, ValueT> ContT;
        ContT cont(builder);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [](ContT &cont, const std::string &k, const int &value)->void
                {
                    auto it = cont.insert(k.c_str(), value);

                    assert(value == cont.size());
                    assert(cont.end() != it);
                    assert(value == *it);
                },
                [](ContT &cont){});

        stat.searchStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [](ContT &cont, const std::string &k, const int &value)->void
                {
                    auto it = cont.find(k.c_str());
                    assert(cont.end() != it);
                    assert(value == *it);
                },
                [](ContT &cont){});
        return stat;
    }

    StatisticData perfHashMap_byString_Test(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "StlHashMap_byString[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        typedef std::unordered_map<std::string, int> ContT;
        ContT cont;
        cont.reserve(keyVals.keysCount_);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [](ContT &cont, const std::string &k, const int &value)->void
                {
                    /// need to create new string, otherwise key will make a copy later
                    cont.insert(ContT::value_type(k.c_str(), value));

                    assert(value == cont.size());
                },
                [](ContT &cont){});

        stat.searchStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [](ContT &cont, const std::string &k, const int &value)->void
                {
                    auto it = cont.find(k.c_str());
                    assert(cont.end() != it);
                },
                [](ContT &cont){});
        return stat;
    }

    StatisticData perfStlHashMap_byStructKeyTest(const KeyValues &keyVals)
    {
        std::stringstream ostr;
        ostr << "StlHashMap_byStructKey[;" << keyVals.subKeyLength_ << ";" << keyVals.keysCount_ << ";]";

        StatisticData stat;
        stat.name_ = ostr.str();

        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder(
                keyVals.lvl1Key_,
                keyVals.lvl2Key_,
                keyVals.lvl3Key_,
                keyVals.lvl4Key_);
        typedef std::unordered_map<ComplexKey, int> ContT;
        ContT cont;
        cont.reserve(keyVals.keysCount_);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        stat.initStat_.memUsage_ = memUsageAfterCreate - memUsageBeforeCreate;

        stat.insertStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [&builder](ContT &cont, const std::string &k, const int &value)->void
                {
                    ContBuilder::ParsedKeyT parsedKey;
                    if(!builder.parseKey(k, parsedKey))
                        throw std::runtime_error("Unable to parse string key to indexes");
                    ComplexKey complexKey = {parsedKey[0], parsedKey[1], parsedKey[2], parsedKey[3]};

                    cont[complexKey] = value;

                    assert(value == cont.size());
                },
                [](ContT &cont){});

        stat.searchStat_ = perfTest<ContT, std::string, int>(
                keyVals,
                cont,
                [&builder](ContT &cont, const std::string &k, const int &)->void
                {
                    ContBuilder::ParsedKeyT parsedKey;
                    if(!builder.parseKey(k, parsedKey))
                        throw std::runtime_error("Unable to parse string key to indexes");
                    ComplexKey complexKey = {parsedKey[0], parsedKey[1], parsedKey[2], parsedKey[3]};

                    auto it = cont.find(complexKey);
                    assert(cont.end() != it);
                },
                [](ContT &cont){});
        return stat;
    }


    void memUsageTestSuite()
    {
        hptimer::setProcessAffinity(3);
        std::vector<size_t> keySizes = {3, 10, 20, 40};

#ifdef DEBUG
        std::vector<KeysCount> keysCounts = {{16, 16, 20, 20}};
#else
        std::vector<KeysCount> keysCounts = {{10, 10, 10, 10}, {16, 16, 20, 20}, {30, 32, 32, 33}, {56, 56, 56, 57}};
#endif

        StatisticsT stats;
        stats.reserve(keySizes.size()*keysCounts.size()*7);
        for(auto keysCount: keysCounts)
        {
            for(auto keySize: keySizes) {
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
                std::cout << "----------------------- SubKeySize = " << vals.subKeyLength_
                          << " , KeysCount = " << vals.keysCount_
                          << " --------------------------------" << std::endl;

                StatisticData st;
                std::cout << "Name;InitMemUsage;InsertMemUsage;SearchMemUsage"<< std::endl;
                st = perfStaticSuffixTreeTest(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_ << ";"
                          << st.insertStat_.memUsage_ << ";"
                          << st.searchStat_.memUsage_ << std::endl;

                st = perfSuffixTreeTest(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_
                          << ";" << st.insertStat_.memUsage_
                          << ";" << st.searchStat_.memUsage_ << std::endl;

                st = perfSuffixTree_adhockKeysTest(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_
                          << ";" << st.insertStat_.memUsage_
                          << ";" << st.searchStat_.memUsage_ << std::endl;

                st = perfStlMap_byString_Test(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_
                          << ";" << st.insertStat_.memUsage_
                          << ";" << st.searchStat_.memUsage_ << std::endl;

                st = perfStlMap_byStructKeyTest(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_
                          << ";" << st.insertStat_.memUsage_
                          << ";" << st.searchStat_.memUsage_ << std::endl;

                st = perfHashMap_byString_Test(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_
                          << ";" << st.insertStat_.memUsage_
                          << ";" << st.searchStat_.memUsage_ << std::endl;

                /*st = perfStlHashMap_byStructKeyTest(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_
                          << ";" << st.insertStat_.memUsage_
                          << ";" << st.searchStat_.memUsage_ << std::endl;*/

                st = perfSortedStlVector_byStringTest(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_
                          << ";" << st.insertStat_.memUsage_
                          << ";" << st.searchStat_.memUsage_ << std::endl;

                st = perfSortedStlVector_byStructTest(vals);
                std::cout << st.name_ << ";" << st.initStat_.memUsage_
                          << ";" << st.insertStat_.memUsage_
                          << ";" << st.searchStat_.memUsage_ << std::endl;
            }
        }

    }

}