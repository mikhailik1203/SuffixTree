//
// Created by sam1203 on 4/11/19.
//

#include "StringArena.h"

#include <stdexcept>
#include <cstring>

using namespace aux;

namespace{
    size_t alignSize(size_t size, unsigned align)
    {
        return ((size >> align) + 1) << align;
    }

    unsigned getAlignBit(unsigned alignBytes)
    {
        switch(alignBytes)
        {
            case 1: return 1;
            case 4: return 2;
            case 8: return 3;
            case 16: return 4;
            case 32: return 5;
            case 64: return 6;
            case 128: return 7;
            default:
                ;
        };
        throw std::logic_error("StringArena doesn't support this alignment");
    }
}

StringArena::StringArena(
        unsigned alignBytes,
        size_t buffer_size,
        double factor,
        size_t limit):
    buffer_(nullptr), sizeLeft_(0), align_(getAlignBit(alignBytes)),
    bufferSize_(buffer_size), factor_(factor), limitSize_(limit)
{
    if(limitSize_ > std::numeric_limits<int>::max())
        throw std::logic_error("KeyArena: LimitSize for arena has to be less 2Gb");
    allocated_.reserve(8);
    allocate(bufferSize_);
}

StringArena::~StringArena()
{
    clear();
}

std::string_view StringArena::allocate(
        const std::string_view &val)
{
    if(val.length() >= sizeLeft_){
        /// allocate new block
        allocate(val.length() + 1);
    }
    memcpy(buffer_, val.data(), val.length());
    buffer_[val.length()] = 0;
    size_t allignedSize = alignSize(val.length() + 1, align_);
    std::string_view res(buffer_, val.length());
    buffer_ += allignedSize;
    sizeLeft_ -= allignedSize;
    return res;
}

void StringArena::allocate(
        size_t size)
{
    size_t sizeToAllocate = std::max(size, static_cast<size_t>(bufferSize_*factor_));
    if(sizeToAllocate < limitSize_){
        bufferSize_ = sizeToAllocate;
    }
    buffer_ = new char[sizeToAllocate];
    sizeLeft_ = static_cast<long long>(sizeToAllocate);
    allocated_.emplace_back(buffer_);
}

void StringArena::clear()
{
    buffer_ = nullptr;
    sizeLeft_ = 0;
    BlocksT tmp;
    std::swap(tmp, allocated_);
}

