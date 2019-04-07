cmake_minimum_required(VERSION 3.13)
project(SuffixTree)

set(CMAKE_CXX_STANDARD 17)

include_directories(.
        ./src
        ./test
        $ENV{BOOST_INCLUDE})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -std=c++17 ") #-DMEM_USAGE_TEST_ -DPERFORMANCE_TEST_
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -DPERFORMANCE_TEST_ -mtune=native -std=c++17 ") #-pg -D__PERF_TEST_ONLY___ -DMEM_USAGE_TEST_
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,realloc")

set(SOURCE_FILES main.cpp ./src/ContBuilder.cpp ./src/hpUtils.cpp ./src/StaticContBuilder.cpp ./src/MemAllocHook.cpp
        ./test/SuffixTreeTest.cpp ./test/StaticSuffixTreeTest.cpp ./test/memUsageTest.cpp ./test/performanceTest.cpp ./test/testUtils.cpp src/ContAllocator.h test/NodeAllocatorTest.cpp)

add_executable(SuffixTree ${SOURCE_FILES})

