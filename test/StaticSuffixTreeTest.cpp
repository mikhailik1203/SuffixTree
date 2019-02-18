#include "StaticSuffixTree.h"
#include "StaticContBuilder.h"
#include "MemAllocHook.h"
#include <cassert>
#include <iostream>
#include <functional>
#include <string>
#include <map>


namespace{
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

}

namespace st_tst{


    void vanillaTest()
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(cont.end() != cont.find("aaa-bbb-ccc-ddd"));
        assert(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }

    void invalidKeyTest()
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-XXX-dda", 777);
        assert(0 == cont.size());
        assert(cont.end() == it);
        assert(cont.end() == cont.find("aaa-bbb-XXX-dda"));
    }

    void findNonexistTest()
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        assert(1 == cont.size());
        assert(cont.end() != it);
        assert(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }

    void insertDuplicateTest()
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
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
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
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
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
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
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
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
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
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
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        assert(0 == cont.size());

        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> contCopy(cont);
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
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
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

        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> contCopy(cont);
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


    void staticContainerTestSuite()
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
    }

}