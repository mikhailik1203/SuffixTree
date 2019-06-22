cmake_minimum_required(VERSION 3.13)
project(SuffixTree)

set(CMAKE_CXX_STANDARD 17)

include_directories(.
        ./src
        ./test
        $ENV{BOOST_INCLUDE})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -std=c++17 ") #-DMEM_USAGE_TEST_ -DPERFORMANCE_TEST_
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native -std=c++17 -DMEM_USAGE_TEST_ ") #-pg -D__PERF_TEST_ONLY___ -DMEM_USAGE_TEST_ -DPERFORMANCE_TEST_

set(SOURCE_FILES main.cpp ./src/hpUtils.cpp ./src/StaticContBuilder.cpp ./src/MemAllocHook.cpp
        ./test/StaticSuffixTreeTest.cpp ./test/memUsageTest.cpp
        ./test/performanceNTest.cpp ./test/testUtils.cpp src/ContAllocator.h test/NodeAllocatorTest.cpp
        src/StringArena.cpp src/StringArena.h src/ContBuilderKeys.cpp src/ContBuilderKeys.h src/SuffixTreeTraits.cpp
        src/SuffixTreeTraits.h test/SuffixTreeNLevelTest.cpp )

# ./test/performanceTest.cpp

add_executable(SuffixTree ${SOURCE_FILES})

