//
// Created by sam1203 on 3/24/19.
//

#if !defined(MEM_USAGE_TEST_) && !defined(PERFORMANCE_TEST_)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wterminate"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "ContAllocator.h"

namespace {
    class TestBuilder{
    public:
        enum SuffixLevel{
            root_TestSuffix = 0,
            first_TestSuffix,
            total_TestSuffix
        };

        template<typename AllocatorT, SuffixLevel>
        struct NodeTraits{
            typedef void KeyTypeT;
            typedef void ParentNodeT;
            typedef void ChildNodeT;
            typedef void NodeTypeT;
        };
    };

    class DestroyCheck{
    public:
        explicit DestroyCheck(bool &isDestroyed):
            isDestroyed_(isDestroyed)
        {}
        ~DestroyCheck(){
            isDestroyed_ = true;
        }

        bool destroyed()const {
            return isDestroyed_;
        }
    private:
        bool &isDestroyed_;
    };

    template<typename AllocatorT>
    struct TestBuilder::NodeTraits<AllocatorT, TestBuilder::root_TestSuffix>{
        typedef DestroyCheck NodeTypeT;
    };

    template<typename AllocatorT>
    struct TestBuilder::NodeTraits<AllocatorT, TestBuilder::first_TestSuffix>{
        typedef DestroyCheck NodeTypeT;
    };

    class FooAlloc{
    public:
    private:
    };
}

BOOST_AUTO_TEST_SUITE( node_alloc_test )

    BOOST_AUTO_TEST_CASE(vanillaTest)
    {
        bool isDestroyed = true;
        suffix_tree::suffix_tree_impl::NodeAllocator<TestBuilder, FooAlloc, TestBuilder::root_TestSuffix> alloc2Test;
        DestroyCheck *obj = alloc2Test.create(isDestroyed);
        BOOST_REQUIRE(nullptr != obj);
        BOOST_REQUIRE(obj->destroyed());
        isDestroyed = false;
        BOOST_REQUIRE(!obj->destroyed());
        alloc2Test.destroy(obj);
        BOOST_REQUIRE(isDestroyed);
    }

    BOOST_AUTO_TEST_CASE(smartPtrTest)
    {
        bool isDestroyed = true;
        suffix_tree::suffix_tree_impl::NodeAllocator<TestBuilder, FooAlloc, TestBuilder::root_TestSuffix> alloc2Test;
        {
            auto obj = alloc2Test.make_unique(isDestroyed);
            BOOST_REQUIRE(nullptr != obj.get());
            BOOST_REQUIRE(obj->destroyed());
            isDestroyed = false;
            BOOST_REQUIRE(!obj->destroyed());
        }
        BOOST_REQUIRE(isDestroyed);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif