//
// Created by sam1203 on 4/11/19.
//

#include "ContBuilderKeys.h"

#include <algorithm>
#include <cstring>
#include <cassert>

using namespace aux;

namespace{
    Key2IndexT toKey2IndexT(const Key2IdxT &vals, StringArena &storage)
    {
        Key2IndexT res;
        std::for_each(
                std::begin(vals), std::end(vals),
                [&](const KeyT &v)
                {
                    std::string_view valCpy = storage.allocate(v);
                    res[valCpy] = res.size();
                });
        return res;
    }

}


ContBuilderKeys::ContBuilderKeys(size_t levelCount):
    stringAllocator_(32, 1024, 2.0, std::numeric_limits<int>::max())
{
    meta_.reserve(levelCount);
    for(size_t i = 0; i < levelCount; ++i){
        meta_.push_back(Key2IndexT());
    }
}

ContBuilderKeys::ContBuilderKeys(
        const Key2IdxT &lvl1,
        const Key2IdxT &lvl2):
        stringAllocator_(32, 1024, 2.0, std::numeric_limits<int>::max())
{
    meta_.reserve(2);
    meta_.emplace_back(toKey2IndexT(lvl1, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl2, stringAllocator_));
}

ContBuilderKeys::ContBuilderKeys(
        const Key2IdxT &lvl1,
        const Key2IdxT &lvl2,
        const Key2IdxT &lvl3):
        stringAllocator_(32, 1024, 2.0, std::numeric_limits<int>::max())
{
    meta_.reserve(3);
    meta_.emplace_back(toKey2IndexT(lvl1, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl2, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl3, stringAllocator_));
}

ContBuilderKeys::ContBuilderKeys(
        const Key2IdxT &lvl1,
        const Key2IdxT &lvl2,
        const Key2IdxT &lvl3,
        const Key2IdxT &lvl4):
    stringAllocator_(32, 1024, 2.0, std::numeric_limits<int>::max())
{
    meta_.reserve(4);
    meta_.emplace_back(toKey2IndexT(lvl1, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl2, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl3, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl4, stringAllocator_));
}

ContBuilderKeys::ContBuilderKeys(
        const Key2IdxT &lvl1,
        const Key2IdxT &lvl2,
        const Key2IdxT &lvl3,
        const Key2IdxT &lvl4,
        const Key2IdxT &lvl5):
        stringAllocator_(32, 1024, 2.0, std::numeric_limits<int>::max())
{
    meta_.reserve(5);
    meta_.emplace_back(toKey2IndexT(lvl1, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl2, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl3, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl4, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl5, stringAllocator_));
}

ContBuilderKeys::ContBuilderKeys(
        const Key2IdxT &lvl1,
        const Key2IdxT &lvl2,
        const Key2IdxT &lvl3,
        const Key2IdxT &lvl4,
        const Key2IdxT &lvl5,
        const Key2IdxT &lvl6):
        stringAllocator_(32, 1024, 2.0, std::numeric_limits<int>::max())
{
    meta_.reserve(6);
    meta_.emplace_back(toKey2IndexT(lvl1, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl2, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl3, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl4, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl5, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl6, stringAllocator_));
}

ContBuilderKeys::ContBuilderKeys(
        const Key2IdxT &lvl1,
        const Key2IdxT &lvl2,
        const Key2IdxT &lvl3,
        const Key2IdxT &lvl4,
        const Key2IdxT &lvl5,
        const Key2IdxT &lvl6,
        const Key2IdxT &lvl7):
        stringAllocator_(32, 1024, 2.0, std::numeric_limits<int>::max())
{
    meta_.reserve(7);
    meta_.emplace_back(toKey2IndexT(lvl1, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl2, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl3, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl4, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl5, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl6, stringAllocator_));
    meta_.emplace_back(toKey2IndexT(lvl7, stringAllocator_));
}

ContBuilderKeys::ContBuilderKeys(const ContBuilderKeys &keys):
    stringAllocator_(32, 1024, 2.0, std::numeric_limits<int>::max())
{
    meta_.reserve(keys.meta_.size());
    for(auto &mit: keys.meta_)
    {
        meta_.emplace_back(Key2IndexT());
        auto &level = meta_.back();
        for(auto &it: mit)
        {
            std::string_view valCpy = stringAllocator_.allocate(it.first);
            level[valCpy] = it.second;
        }
    }

}

ContBuilderKeys &ContBuilderKeys::operator=(const ContBuilderKeys &cont)
{
    if(this == &cont)
        return *this;
    ContBuilderKeys tmp(cont);
    std::swap(meta_, tmp.meta_);
    std::swap(stringAllocator_, tmp.stringAllocator_);
    return *this;
}

size_t ContBuilderKeys::suffixCount(
        size_t level)const noexcept
{
    assert(level < meta_.size());
    return meta_[level].size();
}

const Key2IndexT &ContBuilderKeys::level(size_t level)const noexcept
{
    assert(level < meta_.size());
    return meta_[level];
}

size_t ContBuilderKeys::addKey(size_t level, const KeyViewT &val)
{
    assert(level < meta_.size());
    Key2IndexT &levelKeys = meta_[level];
    size_t index = levelKeys.size();
    KeyViewT cpy = stringAllocator_.allocate(val);
    levelKeys[cpy] = index;
    return index;
}