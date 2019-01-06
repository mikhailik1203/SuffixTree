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

    void fillContainerTest()
    {
        HighPerfTimer timer;
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        size_t memUsageBefore = processMemUsage();
        timer.start();
        suffix_tree::SuffixTree<ContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        char value[] = "aaa-bba-cca-dda";
        int count = 1;
        for(size_t i = 0; i < 26; ++i){
            for(size_t j = 0; j < 26; ++j){
                for(size_t k = 0; k < 26; ++k){
                    for(size_t l = 0; l < 26; ++l){
                        auto it = cont.insert(value, count);

                        assert(count == cont.size());
                        assert(cont.end() != it);
                        assert(count == *it);

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
        std::cout<< "MemUsage for SuffixTree is [" << memUsageAfter - memUsageBefore<< "], before [" << memUsageBefore << "], after [" << memUsageAfter << "]" << std::endl;
        std::cout<< "\tDuration of insert into SuffixTree is [" << timer.interval()<< "]mksec" << std::endl;

        {
            size_t memUsageBefore = processMemUsage();
            timer.start();

            char value[] = "aaa-bba-cca-dda";
            int count = 1;
            for(size_t i = 0; i < 26; ++i){
                for(size_t j = 0; j < 26; ++j){
                    for(size_t k = 0; k < 26; ++k){
                        for(size_t l = 0; l < 26; ++l){
                            auto it = cont.find(value);
                            assert(cont.end() != it);
                            value[14] += 1;
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
            std::cout<< "\tSearch of all elements in SuffixTree is [" << timer.interval()<< "]mksec" << std::endl;
        }
    }

    void fillStlMapContainerTest()
    {
        HighPerfTimer timer;
        size_t memUsageBefore = processMemUsage();
        timer.start();
        std::map<std::string, int> cont;
        assert(0 == cont.size());
        char value[] = "aaa-bba-cca-dda";
        int count = 1;
        for(size_t i = 0; i < 26; ++i){
            for(size_t j = 0; j < 26; ++j){
                for(size_t k = 0; k < 26; ++k){
                    for(size_t l = 0; l < 26; ++l){
                        cont[value] = count;

                        assert(count == cont.size());

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
        std::cout<< "MemUsage for std::map is [" << memUsageAfter - memUsageBefore<< "], before [" << memUsageBefore << "], after [" << memUsageAfter << "]" << std::endl;
        std::cout<< "\tDuration of insert into std::map is [" << timer.interval()<< "]mksec" << std::endl;

        {
            size_t memUsageBefore = processMemUsage();
            timer.start();

            char value[] = "aaa-bba-cca-dda";
            int count = 1;
            for(size_t i = 0; i < 26; ++i){
                for(size_t j = 0; j < 26; ++j){
                    for(size_t k = 0; k < 26; ++k){
                        for(size_t l = 0; l < 26; ++l){
                            auto it = cont.find(value);
                            assert(cont.end() != it);
                            value[14] += 1;
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
            std::cout<< "\tSearch of all elements in std::map is [" << timer.interval()<< "]mksec" << std::endl;
        }

    }

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


    void fillStlComplexMapContainerTest()
    {
        ComplexKey complexKey;
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());

        HighPerfTimer timer;
        size_t memUsageBefore = processMemUsage();
        timer.start();
        std::map<ComplexKey, int> cont;
        assert(0 == cont.size());
        char value[] = "aaa-bba-cca-dda";
        int count = 1;
        for(size_t i = 0; i < 26; ++i){
            for(size_t j = 0; j < 26; ++j){
                for(size_t k = 0; k < 26; ++k){
                    for(size_t l = 0; l < 26; ++l){
                        ContBuilder::ParsedKeyT parsedKey;
                        if(!builder.parseKey(value, parsedKey))
                            throw std::runtime_error("Unable to parse string key to indexes");
                        complexKey.idx1_ = parsedKey[0];
                        complexKey.idx2_ = parsedKey[1];
                        complexKey.idx3_ = parsedKey[2];
                        complexKey.idx4_ = parsedKey[3];
                        cont[complexKey] = count;

                        assert(count == cont.size());

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
        std::cout<< "MemUsage for complex std::map is [" << memUsageAfter - memUsageBefore<< "], before [" << memUsageBefore << "], after [" << memUsageAfter << "]" << std::endl;
        std::cout<< "\tDuration of insert into complex std::map is [" << timer.interval()<< "]mksec" << std::endl;

        {
            size_t memUsageBefore = processMemUsage();
            timer.start();

            char value[] = "aaa-bba-cca-dda";
            int count = 1;
            for(size_t i = 0; i < 26; ++i){
                for(size_t j = 0; j < 26; ++j){
                    for(size_t k = 0; k < 26; ++k){
                        for(size_t l = 0; l < 26; ++l){
                            ContBuilder::ParsedKeyT parsedKey;
                            if(!builder.parseKey(value, parsedKey))
                                throw std::runtime_error("Unable to parse string key to indexes");
                            complexKey.idx1_ = parsedKey[0];
                            complexKey.idx2_ = parsedKey[1];
                            complexKey.idx3_ = parsedKey[2];
                            complexKey.idx4_ = parsedKey[3];

                            auto it = cont.find(complexKey);
                            assert(cont.end() != it);
                            value[14] += 1;
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
            std::cout<< "\tSearch of all elements in complex std::map is [" << timer.interval()<< "]mksec" << std::endl;
        }

    }

    void fillSortedStlVectorContainerTest()
    {
        HighPerfTimer timer;
        size_t memUsageBefore = processMemUsage();
        timer.start();
        typedef std::pair<std::string, int> ValT;
        std::vector<ValT> cont;
        cont.reserve(26*26*26*26);
        assert(0 == cont.size());
        char value[] = "aaa-bba-cca-dda";
        int count = 1;
        for(size_t i = 0; i < 26; ++i){
            for(size_t j = 0; j < 26; ++j){
                for(size_t k = 0; k < 26; ++k){
                    for(size_t l = 0; l < 26; ++l){
                        cont.push_back(ValT(value, count));

                        assert(count == cont.size());

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
        cont.shrink_to_fit();
        std::sort(cont.begin(), cont.end(), [](const ValT &lft, const ValT &rght){ return lft.first < rght.first;});
        timer.stop();
        size_t memUsageAfter = processMemUsage();
        std::cout<< "MemUsage for sorted std::vector is [" << memUsageAfter - memUsageBefore<< "], before [" << memUsageBefore << "], after [" << memUsageAfter << "]" << std::endl;
        std::cout<< "\tDuration of insert into sorted std::vector is [" << timer.interval()<< "]mksec" << std::endl;

        {
            size_t memUsageBefore = processMemUsage();
            timer.start();

            char value[] = "aaa-bba-cca-dda";
            int count = 1;
            for(size_t i = 0; i < 26; ++i){
                for(size_t j = 0; j < 26; ++j){
                    for(size_t k = 0; k < 26; ++k){
                        for(size_t l = 0; l < 26; ++l){
                            auto it = std::lower_bound(cont.begin(), cont.end(), ValT(value, 0), 
                                [](const ValT &lft, const ValT &rght){return lft.first < rght.first;});
                            assert(cont.end() != it);
                            assert(value == it->first);
                            value[14] += 1;
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
            std::cout<< "\tSearch of all elements in sorted std::vector is [" << timer.interval()<< "]mksec" << std::endl;
        }

    }

    void fillComplexSortedStlVectorContainerTest()
    {
        ComplexKey complexKey;
        ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());

        HighPerfTimer timer;
        size_t memUsageBefore = processMemUsage();
        timer.start();
        typedef std::pair<ComplexKey, int> ValT;
        std::vector<ValT> cont;
        cont.reserve(26*26*26*26);
        assert(0 == cont.size());
        char value[] = "aaa-bba-cca-dda";
        int count = 1;
        for(size_t i = 0; i < 26; ++i){
            for(size_t j = 0; j < 26; ++j){
                for(size_t k = 0; k < 26; ++k){
                    for(size_t l = 0; l < 26; ++l){
                        ContBuilder::ParsedKeyT parsedKey;
                        if(!builder.parseKey(value, parsedKey))
                            throw std::runtime_error("Unable to parse string key to indexes");
                        complexKey.idx1_ = parsedKey[0];
                        complexKey.idx2_ = parsedKey[1];
                        complexKey.idx3_ = parsedKey[2];
                        complexKey.idx4_ = parsedKey[3];
                        cont.push_back(ValT(complexKey, count));

                        assert(count == cont.size());

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
        cont.shrink_to_fit();
        std::sort(cont.begin(), cont.end(), [](const ValT &lft, const ValT &rght){ return lft.first < rght.first;});
        timer.stop();
        size_t memUsageAfter = processMemUsage();
        std::cout<< "MemUsage for complex sorted std::vector is [" << memUsageAfter - memUsageBefore<< "], before [" << memUsageBefore << "], after [" << memUsageAfter << "]" << std::endl;
        std::cout<< "\tDuration of insert into complex sorted std::vector is [" << timer.interval()<< "]mksec" << std::endl;

        {
            size_t memUsageBefore = processMemUsage();
            timer.start();

            char value[] = "aaa-bba-cca-dda";
            int count = 1;
            for(size_t i = 0; i < 26; ++i){
                for(size_t j = 0; j < 26; ++j){
                    for(size_t k = 0; k < 26; ++k){
                        for(size_t l = 0; l < 26; ++l){
                            ContBuilder::ParsedKeyT parsedKey;
                            if(!builder.parseKey(value, parsedKey))
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
                            value[14] += 1;
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
            std::cout<< "\tSearch of all elements in complex sorted std::vector is [" << timer.interval()<< "]mksec" << std::endl;
        }

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
        fillContainerTest();
        fillStlMapContainerTest();
        fillStlComplexMapContainerTest();
        fillSortedStlVectorContainerTest();
        fillComplexSortedStlVectorContainerTest();
    }

}