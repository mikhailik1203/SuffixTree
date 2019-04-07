#if !defined(MEM_USAGE_TEST_) && !defined(PERFORMANCE_TEST_)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wterminate"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "SuffixTree.h"
#include "ContBuilder.h"
#include "hpUtils.h"
#include "MemAllocHook.h"
#include <cassert>
#include <iostream>
#include <map>
#include <iomanip>
#include <string>

namespace{
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

    template<typename ContT, typename KeyT, typename ValueT>
    void perfTest(ContT &cont, 
                  std::function<void(ContT &cont, const KeyT &, const ValueT &val)> insertAndCheckFunc, 
                  std::function<void(ContT &cont)> postProcFunc, 
                  const std::string &contName)
    {
        hptimer::HighPerfTimer timer;
        size_t memUsageBefore = mem_alloc::processMemUsage();
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
        postProcFunc(cont);
        timer.stop();
        size_t memUsageAfter = mem_alloc::processMemUsage();
        std::cout<< "\tMemUsage for " << contName << " is [" << memUsageAfter - memUsageBefore << "] bytes" << std::endl;
        std::cout<< "\tDuration of " << contName << " is [" << std::setprecision(12)<< timer.interval()<< "]nsec" << std::endl;
    }

    aux::Key2IdxT prepareLevel1Keys()
    {
        aux::Key2IdxT res;
        char value[] = "aaa";
        for(size_t i = 0; i < 26; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    aux::Key2IdxT prepareLevel2Keys()
    {
        aux::Key2IdxT res;
        char value[] = "bba";
        for(size_t i = 0; i < 26; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    aux::Key2IdxT prepareLevel3Keys()
    {
        aux::Key2IdxT res;
        char value[] = "cca";
        for(size_t i = 0; i < 26; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    aux::Key2IdxT prepareLevel4Keys()
    {
        aux::Key2IdxT res;
        char value[] = "dda";
        for(size_t i = 0; i < 26; ++i){
            res.push_back(value);        
            value[2] += 1;
        }
        std::sort(res.begin(), res.end());
        return res;
    }

    typedef std::pair<double, double> MinMaxPairT;

    template<typename ContT, typename KeyT, typename ValueT>
    MinMaxPairT latencyTest(
                  ContT &cont, 
                  const GeneratedKeyT &keys,
                  size_t batchSize,
                  std::function<void(ContT &, const KeyT &, const ValueT &)> insertAndCheckFunc, 
                  std::function<void(ContT &)> postProcFunc, 
                  const std::string &contName)
    {
        size_t count = keys.size();
        DurationsT dur(count, 0.0);
        hptimer::HighPerfTimer timer;
        double minL = 1000000000, maxL = 0;
        size_t i = 0;
        while(i < count){
            timer.start();
            for(size_t j = 0; j < batchSize; ++j){
                insertAndCheckFunc(cont, keys[i + j], static_cast<int>(count));
            }
            timer.stop();
            double v = timer.interval();
            dur[i] = v;
            minL = std::min(minL, v);
            maxL = std::max(maxL, v);
            i += batchSize;
        }
        return MinMaxPairT(minL, maxL);
    }

}

BOOST_AUTO_TEST_SUITE( suffix_tree_test )


    BOOST_AUTO_TEST_CASE(vanillaTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT > cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(cont.end() != cont.find("aaa-bbb-ccc-ddd"));
        assert(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }

    BOOST_AUTO_TEST_CASE(invalidKeyTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
        assert(0 == cont.size());
        /// key with 3 subkeys, while 4 subkeys are expected
        auto it = cont.insert("aaa-bbb-dda", 777);
        assert(0 == cont.size());
        assert(cont.end() == it);
        assert(cont.end() == cont.find("aaa-bbb-dda"));
        assert(cont.end() == cont.erase("aaa-bbb-dda"));
    }

    BOOST_AUTO_TEST_CASE(findNonexistTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }

    BOOST_AUTO_TEST_CASE(insertDuplicateTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
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

    BOOST_AUTO_TEST_CASE(eraseByKeyTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(777 == it.value());
        cont.erase("aaa-bbb-ccc-ddd");
        assert(0 == cont.size());
    }

    BOOST_AUTO_TEST_CASE(eraseAgainByKeyTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
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

    BOOST_AUTO_TEST_CASE(eraseByIteratorTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
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

    BOOST_AUTO_TEST_CASE(setByIteratorTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
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

    BOOST_AUTO_TEST_CASE(beginIteratorTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
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

        auto itBegin = cont.begin();
        assert(3 == cont.size());
        assert(cont.end() != itBegin);
        assert(333 == itBegin.value());
    }

    BOOST_AUTO_TEST_CASE(copyEmptyContainerTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
        assert(0 == cont.size());

        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> contCopy(cont);
        assert(0 == contCopy.size());

        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(777 == *it);

        auto it1 = contCopy.find("aaa-bbb-ccc-ddd");
        assert(contCopy.end() == it1);
    }

    BOOST_AUTO_TEST_CASE(copyContainerTest)
    {
        aux::ContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
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

        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> contCopy(cont);
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

    BOOST_AUTO_TEST_CASE(addNewKeyTest)
    {
        aux::ContBuilder builder;
        suffix_tree::SuffixTree<aux::ContBuilder, std::string, int, aux::ContNodeAllocatorsT> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("new-sub-key-test", 1234);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(cont.end() != cont.find("new-sub-key-test"));
        assert(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }


BOOST_AUTO_TEST_SUITE_END()

#endif
