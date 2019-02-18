#include <iostream>

namespace tst{
    extern void containerTestSuite();
    extern void memUsageTestSuite();
    extern void performanceTestSuite();
}

namespace st_tst{
    extern void staticContainerTestSuite();
}

int main() {
    tst::containerTestSuite();
    st_tst::staticContainerTestSuite();
#ifdef MEM_USAGE_TEST_
    tst::memUsageTestSuite();
#else
    tst::performanceTestSuite();
#endif
    return 0;
}