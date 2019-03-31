#if !defined(MEM_USAGE_TEST_) && !defined(PERFORMANCE_TEST_)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wterminate"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "StaticSuffixTree.h"
#include "StaticContBuilder.h"
#include "MemAllocHook.h"
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

BOOST_AUTO_TEST_SUITE( static_suffix_tree_test )


    BOOST_AUTO_TEST_CASE (vanillaTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it);
        BOOST_REQUIRE(cont.end() != cont.find("aaa-bbb-ccc-ddd"));
        BOOST_REQUIRE(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }

    BOOST_AUTO_TEST_CASE (invalidKeyTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it = cont.insert("aaa-bbb-XXX-dda", 777);
        BOOST_REQUIRE(0 == cont.size());
        BOOST_REQUIRE(cont.end() == it);
        BOOST_REQUIRE(cont.end() == cont.find("aaa-bbb-XXX-dda"));
    }

    BOOST_AUTO_TEST_CASE (findNonexistTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it);
        BOOST_REQUIRE(cont.end() == cont.find("aaa-bbb-cca-ddd"));
    }

    BOOST_AUTO_TEST_CASE (insertDuplicateTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it);
        BOOST_REQUIRE(777 == it.value());
        auto it2 = cont.insert("aaa-bbb-ccc-ddd", 888);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it2);
        BOOST_REQUIRE(888 == it2.value());
    }

    BOOST_AUTO_TEST_CASE (eraseByKeyTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it);
        BOOST_REQUIRE(777 == it.value());
        cont.erase("aaa-bbb-ccc-ddd");
        BOOST_REQUIRE(0 == cont.size());
    }

    BOOST_AUTO_TEST_CASE (eraseAgainByKeyTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it);
        BOOST_REQUIRE(777 == it.value());
        it = cont.erase("aaa-bbb-ccc-ddd");
        BOOST_REQUIRE(0 == cont.size());
        BOOST_REQUIRE(cont.end() == it);
        it = cont.erase("aaa-bbb-ccc-ddd");
        BOOST_REQUIRE(0 == cont.size());
        BOOST_REQUIRE(cont.end() == it);
    }

    BOOST_AUTO_TEST_CASE (eraseByIteratorTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it);
        BOOST_REQUIRE(777 == it.value());

        auto it2 = cont.insert("aaa-bba-cca-dda", 333);
        BOOST_REQUIRE(2 == cont.size());
        BOOST_REQUIRE(cont.end() != it2);
        BOOST_REQUIRE(333 == it2.value());

        auto it3 = cont.insert("aaz-bbz-ccz-ddz", 999);
        BOOST_REQUIRE(3 == cont.size());
        BOOST_REQUIRE(cont.end() != it3);
        BOOST_REQUIRE(999 == it3.value());

        auto itAfterErase = cont.erase(cont.end());
        BOOST_REQUIRE(3 == cont.size());
        BOOST_REQUIRE(cont.end() == itAfterErase);

        itAfterErase = cont.erase(it);
        BOOST_REQUIRE(2 == cont.size());
        BOOST_REQUIRE(cont.end() != itAfterErase);
        BOOST_REQUIRE(999 == itAfterErase.value());

        itAfterErase = cont.erase(it3);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() == itAfterErase);

        itAfterErase = cont.erase(it2);
        BOOST_REQUIRE(0 == cont.size());
        BOOST_REQUIRE(cont.end() == itAfterErase);
    }

    BOOST_AUTO_TEST_CASE (setByIteratorTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it);
        BOOST_REQUIRE(777 == *it);

        *it = 7778;
        BOOST_REQUIRE(7778 == *it);
        auto it1 = cont.find("aaa-bbb-ccc-ddd");
        BOOST_REQUIRE(cont.end() != it1);
        BOOST_REQUIRE(7778 == *it1);
    }

    BOOST_AUTO_TEST_CASE (copyEmptyContainerTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());

        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> contCopy(cont);
        BOOST_REQUIRE(0 == contCopy.size());

        auto it = cont.insert("aaa-bbb-ccc-ddd", 777);
        BOOST_REQUIRE(1 == cont.size());
        BOOST_REQUIRE(cont.end() != it);
        BOOST_REQUIRE(777 == *it);

        auto it1 = contCopy.find("aaa-bbb-ccc-ddd");
        BOOST_REQUIRE(contCopy.end() == it1);
    }

    BOOST_AUTO_TEST_CASE (copyContainerTest)
    {
        StaticContBuilder builder(prepareLevel1Keys(), prepareLevel2Keys(), prepareLevel3Keys(), prepareLevel4Keys());
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> cont(builder);
        BOOST_REQUIRE(0 == cont.size());
        auto it2 = cont.insert("aaa-bbb-ccc-ddd", 777);
        auto it1 = cont.insert("aaa-bba-cca-dda", 1111);
        auto it3 = cont.insert("aaz-bba-cca-dda", 8888);
        auto it4 = cont.insert("aaz-bbz-ccz-ddz", 99);
        BOOST_REQUIRE(4 == cont.size());
        BOOST_REQUIRE(cont.end() != it1);
        BOOST_REQUIRE(1111 == *it1);
        BOOST_REQUIRE(cont.end() != it2);
        BOOST_REQUIRE(777 == *it2);
        BOOST_REQUIRE(cont.end() != it3);
        BOOST_REQUIRE(8888 == *it3);
        BOOST_REQUIRE(cont.end() != it4);
        BOOST_REQUIRE(99 == *it4);

        /// make a copy
        st_suffix_tree::StaticSuffixTree<StaticContBuilder, std::string, int> contCopy(cont);
        BOOST_REQUIRE(4 == contCopy.size());

        /// update values in original container
        *it1 = 11112;
        *it2 = 7772;
        *it3 = 88882;
        *it4 = 992;

        /// copy of original contains old values
        auto cit = contCopy.find("aaa-bba-cca-dda");
        BOOST_REQUIRE(contCopy.end() != cit);
        BOOST_REQUIRE(1111 == *cit);
        cit = contCopy.find("aaa-bbb-ccc-ddd");
        BOOST_REQUIRE(contCopy.end() != cit);
        BOOST_REQUIRE(777 == *cit);
        cit = contCopy.find("aaz-bba-cca-dda");
        BOOST_REQUIRE(contCopy.end() != cit);
        BOOST_REQUIRE(8888 == *cit);
        cit = contCopy.find("aaz-bbz-ccz-ddz");
        BOOST_REQUIRE(contCopy.end() != cit);
        BOOST_REQUIRE(99 == *cit);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif