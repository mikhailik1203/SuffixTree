#if !defined(MEM_USAGE_TEST_) && !defined(PERFORMANCE_TEST_)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SuffixTreeTest
// include this to get main() and UTF included by headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wterminate"
#include <boost/test/included/unit_test.hpp>
#pragma GCC diagnostic pop

#else

#include <iostream>

namespace tst{
    extern void memUsageTestSuite();
    extern void performanceTestSuite();
}

int main() {
#if defined(MEM_USAGE_TEST_)
    tst::memUsageTestSuite();
#endif

#if defined(PERFORMANCE_TEST_)
    tst::performanceTestSuite();
#endif

    return 0;
}

#endif
