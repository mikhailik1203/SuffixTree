#include "stdafx.h"
#include "MemAllocHook.h"
#include "SuffixTree.h"
#include "ContBuilder.h"
#include "StaticSuffixTree.h"
#include "StaticContBuilder.h"
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
    const size_t LEVEL_1_KEYS_COUNT = 56;//16;//30;//56;//30
    const size_t LEVEL_2_KEYS_COUNT = 56;//16;//32;//56;//32
    const size_t LEVEL_3_KEYS_COUNT = 56;//20;//32;//56;//32
    const size_t LEVEL_4_KEYS_COUNT = 57;//20;//33;//57;//33
#endif
    const char KEY_START_SYMBOL = 'A';

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
    void perfTest(ContT &cont, 
                  std::function<void(ContT &cont, const KeyT &, const ValueT &val)> insertAndCheckFunc, 
                  std::function<void(ContT &cont)> postProcFunc, 
                  const std::string &contName)
    {
        GeneratedKeyT keys = generateKeys();

        hptimer::HighPerfTimer timer;
        size_t memUsageBefore = mem_alloc::processMemUsage();
        timer.start();
        int count = 1;
        for(size_t i = 0; i < keys.size(); ++i){
            insertAndCheckFunc(cont, keys[i], static_cast<int>(i + 1));
        }
        postProcFunc(cont);
        timer.stop();
        size_t memUsageAfter = mem_alloc::processMemUsage();
        std::cout<< "\tMemUsage for " << contName << " is [" << memUsageAfter - memUsageBefore << "] bytes" << std::endl;
        std::cout<< "\tDuration of " << contName << " is [" << std::setprecision(12)<< timer.interval()<< "]" << timer.units() << std::endl;
    }
}

namespace tst{


    void perfSuffixTreeTest()
    {
        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        size_t memUsageBefore = mem_alloc::processMemUsage();
        typedef suffix_tree::SuffixTree<ContBuilder, std::string, int> ContT;
        ContT cont(builder);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        std::cout<< "Performance of SuffixTree: "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate << "]bytes" << std::endl;

        perfTest<ContT, std::string, int>(
            cont, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                auto it = cont.insert(key, value);

                assert(value == cont.size());
                assert(cont.end() != it);
                assert(value == *it);
            }, 
            [](ContT &cont){},
            "inserts into");

        perfTest<ContT, std::string, int>(
            cont, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                auto it = cont.find(key);
                assert(cont.end() != it);
                assert(value == *it);
            }, 
            [](ContT &cont){},
            "seq search");
    }

    void perfSuffixTree_adhockKeysTest()
    {
        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder;
        size_t memUsageBefore = mem_alloc::processMemUsage();
        typedef suffix_tree::SuffixTree<ContBuilder, std::string, int> ContT;
        ContT cont(builder);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        std::cout<< "Performance of SuffixTree(adhock keys): "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate << "]bytes" << std::endl;

        perfTest<ContT, std::string, int>(
            cont, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                auto it = cont.insert(key, value);

                assert(value == cont.size());
                assert(cont.end() != it);
                assert(value == *it);
            }, 
            [](ContT &cont){},
            "inserts into");

        perfTest<ContT, std::string, int>(
            cont, 
            [](ContT &cont, const std::string& key, const int &value)->void
            {
                auto it = cont.find(key);
                assert(cont.end() != it);
                assert(value == *it);
            }, 
            [](ContT &cont){},
            "seq search");
    }

    void perfStlMap_byString_Test()
    {
        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        typedef std::map<std::string, int> ContT;
        ContT cont;
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        std::cout<< "Performance of std::map: "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes" << std::endl;

        perfTest<ContT, std::string, int>(
            cont, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                cont[key] = value;

                assert(value == cont.size());
            }, 
            [](ContT &cont){},
            "inserts into");

        perfTest<ContT, std::string, int>(
            cont, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                auto it = cont.find(key);
                assert(cont.end() != it);
            }, 
            [](ContT &cont){},
            "seq search");
    }

    void perfStlMap_byStructKeyTest()
    {
        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        typedef std::map<ComplexKey, int> ContT;
        ContT cont;
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        std::cout<< "Performance of std::map(struct key): "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes" << std::endl;

        perfTest<ContT, std::string, int>(
            cont, 
            [&builder](ContT &cont, const std::string &key, const int &value)->void
            {
                ComplexKey complexKey;
                ContBuilder::ParsedKeyT parsedKey;
                if(!builder.parseKey(key, parsedKey))
                    throw std::runtime_error("Unable to parse string key to indexes");
                complexKey.idx1_ = parsedKey[0];
                complexKey.idx2_ = parsedKey[1];
                complexKey.idx3_ = parsedKey[2];
                complexKey.idx4_ = parsedKey[3];
                cont[complexKey] = value;

                assert(value == cont.size());
            }, 
            [](ContT &cont){},
            "inserts into");

        perfTest<ContT, std::string, int>(
            cont, 
            [&builder](ContT &cont, const std::string &key, const int &)->void
            {
                ComplexKey complexKey;
                ContBuilder::ParsedKeyT parsedKey;
                if(!builder.parseKey(key, parsedKey))
                    throw std::runtime_error("Unable to parse string key to indexes");
                complexKey.idx1_ = parsedKey[0];
                complexKey.idx2_ = parsedKey[1];
                complexKey.idx3_ = parsedKey[2];
                complexKey.idx4_ = parsedKey[3];

                auto it = cont.find(complexKey);
                assert(cont.end() != it);
            }, 
            [](ContT &cont){},
            "seq search");
    }

    void perfSortedStlVector_byStringTest()
    {
        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();

        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        typedef std::pair<std::string, int> ValT;
        typedef std::vector<ValT> ContT;
        ContT cont;
        cont.reserve(LEVEL_1_KEYS_COUNT*LEVEL_2_KEYS_COUNT*LEVEL_3_KEYS_COUNT*LEVEL_4_KEYS_COUNT);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        std::cout<< "Performance of sorted std::vector(string key): "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes" << std::endl;

        perfTest<ContT, std::string, int>(
            cont, 
            [&builder](ContT &cont, const std::string &key, const int &value)->void
            {
                cont.push_back(ValT(key, value));
                assert(value == cont.size());
            }, 
            [](ContT &cont){
                cont.shrink_to_fit();
                std::sort(cont.begin(), cont.end(), [](const ValT &lft, const ValT &rght){ return lft.first < rght.first;});
            },
            "inserts into");

        perfTest<ContT, std::string, int>(
            cont, 
            [&builder](ContT &cont, const std::string &key, const int &)->void
            {
                auto it = std::lower_bound(cont.begin(), cont.end(), ValT(key, 0), 
                    [](const ValT &lft, const ValT &rght){return lft.first < rght.first;});
                assert(cont.end() != it);
                assert(key == it->first);
            }, 
            [](ContT &cont){},
            "seq search");
    }

    void perfSortedStlVector_byStructTest()
    {
        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        typedef std::pair<ComplexKey, int> ValT;
        typedef std::vector<ValT> ContT;
        ContT cont;
        cont.reserve(LEVEL_1_KEYS_COUNT*LEVEL_2_KEYS_COUNT*LEVEL_3_KEYS_COUNT*LEVEL_4_KEYS_COUNT);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        std::cout<< "Performance of sorted std::vector(struct key): "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate << "]bytes" << std::endl;

        perfTest<ContT, std::string, int>(
            cont, 
            [&builder](ContT &cont, const std::string &key, const int &value)->void
            {
                ComplexKey complexKey;
                ContBuilder::ParsedKeyT parsedKey;
                if(!builder.parseKey(key, parsedKey))
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
            },
            "inserts into");

        perfTest<ContT, std::string, int>(
            cont, 
            [&builder](ContT &cont, const std::string &key, const int &)->void
            {
                ComplexKey complexKey;
                ContBuilder::ParsedKeyT parsedKey;
                if(!builder.parseKey(key, parsedKey))
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
            [](ContT &cont){},
            "seq search");
    }

    void perfStaticSuffixTreeTest()
    {
        size_t memUsageBeforeCreate = mem_alloc::processMemUsage();
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        typedef int ValueT;
        typedef st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, ValueT> ContT;
        ContT cont(builder);
        size_t memUsageAfterCreate = mem_alloc::processMemUsage();
        std::cout<< "Performance of StaticSuffixTree: "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes" << std::endl;

        perfTest<ContT, std::string, int>(
            cont, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                auto it = cont.insert(key, value);

                assert(value == cont.size());
                assert(cont.end() != it);
                assert(value == *it);
            }, 
            [](ContT &cont){},
            "inserts into");

        perfTest<ContT, std::string, int>(
            cont, 
            [](ContT &cont, const std::string &key, const int &value)->void
            {
                auto it = cont.find(key);
                assert(cont.end() != it);
                assert(value == *it);
            }, 
            [](ContT &cont){},
            "seq search");
    }

    void memUsageTestSuite()
    {
        hptimer::setProcessAffinity(3);
        perfSuffixTreeTest();
        perfSuffixTree_adhockKeysTest();
        perfStlMap_byString_Test();
        perfStlMap_byStructKeyTest();
        perfSortedStlVector_byStringTest();
        perfSortedStlVector_byStructTest();
        perfStaticSuffixTreeTest();
    }

}