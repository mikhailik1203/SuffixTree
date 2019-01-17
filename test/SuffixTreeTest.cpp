#include "stdafx.h"
#include "SuffixTree.h"
#include "ContBuilder.h"
#include <cassert>
#include <iostream>
#include <map>


namespace{
    size_t processMemUsage()
    {
        PROCESS_MEMORY_COUNTERS memusage;
        GetProcessMemoryInfo(GetCurrentProcess(), &memusage, sizeof(memusage));        
        return memusage.WorkingSetSize;
    }

    class HighPerfTimer{
    public:
        HighPerfTimer()
        {
            QueryPerformanceFrequency(&timerFreq_); 
        }

        void start()
        {
            QueryPerformanceCounter(&startTime_);
        }
        void stop()
        {
            QueryPerformanceCounter(&finishTime_);
        }

        double interval()const
        {
            return (finishTime_.QuadPart - startTime_.QuadPart)*1000000/timerFreq_.QuadPart;
        }
    private:
        LARGE_INTEGER startTime_, finishTime_;
        LARGE_INTEGER timerFreq_;
    };

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

    template<typename ContT, typename KeyT, typename ValueT>
    void perfTest(ContT &cont, 
                  std::function<void(ContT &cont, const KeyT &, const ValueT &val)> insertAndCheckFunc, 
                  std::function<void(ContT &cont)> postProcFunc, 
                  const std::string &contName)
    {
        HighPerfTimer timer;
        size_t memUsageBefore = processMemUsage();
        timer.start();
        char value[] = "aaa-bba-cca-dda";
        int count = 1;
        for(size_t i = 0; i < 26; ++i){
            for(size_t j = 0; j < 26; ++j){
                for(size_t k = 0; k < 26; ++k){
                    for(size_t l = 0; l < 26; ++l){
                        insertAndCheckFunc(cont, value, count);
                        value[14] += 1;
                        ++count;
                    }
                    value[10] += 1;
                    value[14] = 'a';
                }
                value[6] += 1;
                value[10] = 'a';
                value[14] = 'a';
            }
            value[2] += 1;
            value[6] = 'a';
            value[10] = 'a';
            value[14] = 'a';
        }
        timer.stop();
        size_t memUsageAfter = processMemUsage();
        std::cout<< "\tMemUsage for " << contName << " is [" << memUsageAfter - memUsageBefore << "] bytes" << std::endl;
        std::cout<< "\tDuration of " << contName << " is [" << timer.interval()<< "]mksec" << std::endl;
    }


}

namespace tst{

    Key2IdxT prepareLevel1Keys()
    {
        Key2IdxT res;
        char value[] = "aaa";
        for(size_t i = 0; i < 26; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    Key2IdxT prepareLevel2Keys()
    {
        Key2IdxT res;
        char value[] = "bba";
        for(size_t i = 0; i < 26; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    Key2IdxT prepareLevel3Keys()
    {
        Key2IdxT res;
        char value[] = "cca";
        for(size_t i = 0; i < 26; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    Key2IdxT prepareLevel4Keys()
    {
        Key2IdxT res;
        char value[] = "dda";
        for(size_t i = 0; i < 26; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    void vanillaTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(cont.end() != cont.find("aaa-bbb-ccc-ddd"));
        assert(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }

    void invalidKeyTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-XXX-dda", 777);
        assert(0 == cont.size());
        assert(cont.end() == it);
        assert(cont.end() == cont.find("aaa-bbb-XXX-dda"));
    }

    void findNonexistTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }

    void insertDuplicateTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(777 == it.value());
        auto it2 = cont.insert("aaa-bbb-ccc-ddd", 888);
        assert(1 == cont.size());
        assert(cont.end() != it2);
        assert(888 == it2.value());
    }

    void eraseByKeyTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(777 == it.value());
        cont.erase("aaa-bbb-ccc-ddd");
        assert(0 == cont.size());
    }

    void eraseAgainByKeyTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(777 == it.value());
        it = cont.erase("aaa-bbb-ccc-ddd");
        assert(0 == cont.size());
        assert(cont.end() == it);
        it = cont.erase("aaa-bbb-ccc-ddd");
        assert(0 == cont.size());
        assert(cont.end() == it);
    }

    void eraseByIteratorTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(777 == it.value());

        auto it2 = cont.insert("aaa-bba-cca-dda", 333);
        assert(2 == cont.size());
        assert(cont.end() != it2);
        assert(333 == it2.value());

        auto it3 = cont.insert("aaz-bbz-ccz-ddz", 999);
        assert(3 == cont.size());
        assert(cont.end() != it3);
        assert(999 == it3.value());

        auto itAfterErase = cont.erase(cont.end());
        assert(3 == cont.size());
        assert(cont.end() == itAfterErase);

        itAfterErase = cont.erase(it);
        assert(2 == cont.size());
        assert(cont.end() != itAfterErase);
        assert(999 == itAfterErase.value());

        itAfterErase = cont.erase(it3);
        assert(1 == cont.size());
        assert(cont.end() == itAfterErase);

        itAfterErase = cont.erase(it2);
        assert(0 == cont.size());
        assert(cont.end() == itAfterErase);
    }

    void setByIteratorTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(777 == *it);

        *it = 7778;
        assert(7778 == *it);
        auto it1 = cont.find("aaa-bbb-ccc-ddd");
        assert(cont.end() != it1);
        assert(7778 == *it1);
    }

    void copyEmptyContainerTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());

        suffix_tree::SuffixTree<ContBuilder, std::string, int> contCopy(cont);
        assert(0 == contCopy.size());

        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(777 == *it);

        auto it1 = contCopy.find("aaa-bbb-ccc-ddd");
        assert(contCopy.end() == it1);
    }

    void copyContainerTest()
    {
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it2 = cont.insert("aaa-bbb-ccc-ddd", 777);
        auto it1 = cont.insert("aaa-bba-cca-dda", 1111);
        auto it3 = cont.insert("aaz-bba-cca-dda", 8888);
        auto it4 = cont.insert("aaz-bbz-ccz-ddz", 99);
        assert(4 == cont.size());
        assert(cont.end() != it1);
        assert(1111 == *it1);
        assert(cont.end() != it2);
        assert(777 == *it2);
        assert(cont.end() != it3);
        assert(8888 == *it3);
        assert(cont.end() != it4);
        assert(99 == *it4);

        suffix_tree::SuffixTree<ContBuilder, std::string, int> contCopy(cont);
        assert(4 == contCopy.size());
        *it1 = 11112;
        *it2 = 7772;
        *it3 = 88882;
        *it4 = 992;

        auto cit = contCopy.find("aaa-bba-cca-dda");
        assert(contCopy.end() != cit);
        assert(1111 == *cit);
        cit = contCopy.find("aaa-bbb-ccc-ddd");
        assert(contCopy.end() != cit);
        assert(777 == *cit);
        cit = contCopy.find("aaz-bba-cca-dda");
        assert(contCopy.end() != cit);
        assert(8888 == *cit);
        cit = contCopy.find("aaz-bbz-ccz-ddz");
        assert(contCopy.end() != cit);
        assert(99 == *cit);
    }

    void perfSuffixTreeTest()
    {
        size_t memUsageBeforeCreate = processMemUsage();
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        size_t memUsageBefore = processMemUsage();
        typedef suffix_tree::SuffixTree<ContBuilder, std::string, int> ContT;
        ContT cont(builder);
        size_t memUsageAfterCreate = processMemUsage();
        std::cout<< "Performance of SuffixTree: "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes, before [" << memUsageBeforeCreate << "], after [" << memUsageAfterCreate << "]" << std::endl;

        perfTest<ContT, char *, int>(
            cont, 
            [](ContT &cont, const char *key, const int &value)->void
            {
                auto it = cont.insert(key, value);

                assert(value == cont.size());
                assert(cont.end() != it);
                assert(value == *it);
            }, 
            [](ContT &cont){},
            "inserts into");

        perfTest<ContT, char *, int>(
            cont, 
            [](ContT &cont, const char *key, const int &value)->void
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
        size_t memUsageBeforeCreate = processMemUsage();
        typedef std::map<std::string, int> ContT;
        ContT cont;
        size_t memUsageAfterCreate = processMemUsage();
        std::cout<< "Performance of std::map: "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes, before [" << memUsageBeforeCreate << "], after [" << memUsageAfterCreate << "]" << std::endl;

        perfTest<ContT, char *, int>(
            cont, 
            [](ContT &cont, const char *key, const int &value)->void
            {
                cont[key] = value;

                assert(value == cont.size());
            }, 
            [](ContT &cont){},
            "inserts into");

        perfTest<ContT, char *, int>(
            cont, 
            [](ContT &cont, const char *key, const int &value)->void
            {
                auto it = cont.find(key);
                assert(cont.end() != it);
            }, 
            [](ContT &cont){},
            "seq search");
    }

    void perfStlMap_byStructKeyTest()
    {
        size_t memUsageBeforeCreate = processMemUsage();
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        typedef std::map<ComplexKey, int> ContT;
        ContT cont;
        size_t memUsageAfterCreate = processMemUsage();
        std::cout<< "Performance of std::map(struct key): "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes, before [" << memUsageBeforeCreate << "], after [" << memUsageAfterCreate << "]" << std::endl;

        perfTest<ContT, char *, int>(
            cont, 
            [&builder](ContT &cont, const char *key, const int &value)->void
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

        perfTest<ContT, char *, int>(
            cont, 
            [&builder](ContT &cont, const char *key, const int &)->void
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
        size_t memUsageBeforeCreate = processMemUsage();

        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        typedef std::pair<std::string, int> ValT;
        typedef std::vector<ValT> ContT;
        ContT cont;
        cont.reserve(26*26*26*26);
        size_t memUsageAfterCreate = processMemUsage();
        std::cout<< "Performance of sorted std::vector(string key): "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes, before [" << memUsageBeforeCreate << "], after [" << memUsageAfterCreate << "]" << std::endl;

        perfTest<ContT, char *, int>(
            cont, 
            [&builder](ContT &cont, const char *key, const int &value)->void
            {
                cont.push_back(ValT(key, value));
                assert(value == cont.size());
            }, 
            [](ContT &cont){
                cont.shrink_to_fit();
                std::sort(cont.begin(), cont.end(), [](const ValT &lft, const ValT &rght){ return lft.first < rght.first;});
            },
            "inserts into");

        perfTest<ContT, char *, int>(
            cont, 
            [&builder](ContT &cont, const char *key, const int &)->void
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
        size_t memUsageBeforeCreate = processMemUsage();
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        typedef std::pair<ComplexKey, int> ValT;
        typedef std::vector<ValT> ContT;
        ContT cont;
        cont.reserve(26*26*26*26);
        size_t memUsageAfterCreate = processMemUsage();
        std::cout<< "Performance of soreted std::vector(struct key): "<< std::endl;
        std::cout<< "\tMemUsage for creation is [" << memUsageAfterCreate - memUsageBeforeCreate
                 << "]bytes, before [" << memUsageBeforeCreate << "], after [" << memUsageAfterCreate << "]" << std::endl;

        perfTest<ContT, char *, int>(
            cont, 
            [&builder](ContT &cont, const char *key, const int &value)->void
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

        perfTest<ContT, char *, int>(
            cont, 
            [&builder](ContT &cont, const char *key, const int &)->void
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


    void containerTestSuite()
    {
        vanillaTest();
        invalidKeyTest();
        findNonexistTest();
        insertDuplicateTest();
        eraseByKeyTest();
        eraseAgainByKeyTest();
        eraseByIteratorTest();
        setByIteratorTest();
        copyEmptyContainerTest();
        copyContainerTest();

        perfSuffixTreeTest();
        perfStlMap_byString_Test();
        perfStlMap_byStructKeyTest();
        perfSortedStlVector_byStringTest();
        perfSortedStlVector_byStructTest();
    }

}