#include "StdAfx.h"
#include "StaticContBuilder.h"

using namespace suffix_tree;

StaticContBuilder::StaticContBuilder(
        const Key2IdxT &lvl1, 
        const Key2IdxT &lvl2, 
        const Key2IdxT &lvl3, 
        const Key2IdxT &lvl4):
    delimeter_('-')
{
    meta_.reserve(4);
    meta_.push_back(lvl1);
    meta_.push_back(lvl2);
    meta_.push_back(lvl3);
    meta_.push_back(lvl4);
    std::sort(meta_[0].begin(), meta_[0].end());
    std::sort(meta_[1].begin(), meta_[1].end());
    std::sort(meta_[2].begin(), meta_[2].end());
    std::sort(meta_[3].begin(), meta_[3].end());
}

StaticContBuilder::~StaticContBuilder()
{}

size_t StaticContBuilder::suffixCount(
            SuffixLevel level)const
{
    return meta_[level].size();
}

bool StaticContBuilder::getKeyIndex(
            size_t level, 
            const KeyT &key, 
            size_t startIdx, 
            size_t endIdx, 
            suffix_tree::suffix_tree_impl::IndexT &index)const
{
    const Key2IdxT &levelKeys = meta_[level];
    const char *pVal = key.c_str() + startIdx;
    size_t count = endIdx - startIdx;
    auto keyIt = std::lower_bound(
            std::begin(levelKeys), std::end(levelKeys), key, 
            [&](const Key2IdxT::value_type &lft, const KeyT &key)->bool
            {
                return 0 > strncmp(lft.c_str(), pVal, count);
            });
    if(std::end(levelKeys) == keyIt || 
       0 != strncmp(keyIt->c_str(), pVal, count))
        return false;
    index = keyIt - std::begin(levelKeys);
    return true;
}

bool StaticContBuilder::parseKey(
            const KeyT &key, 
            StaticContBuilder::ParsedKeyT &res)const
{
    size_t currLevel = 0;
    size_t startIdx = 0;
    size_t totalLen = key.length();
    for(size_t i = 0; i < totalLen; ++i){
        if(delimeter_ == key[i]){
            suffix_tree::suffix_tree_impl::IndexT index = 0;
            if(!getKeyIndex(currLevel, key, startIdx, i, index))
                return false;
            res[currLevel] = index;
            ++currLevel;
            startIdx = i + 1; ///skip delimeter
        }
    }
    if(startIdx < totalLen){
        suffix_tree::suffix_tree_impl::IndexT index = 0;
        if(!getKeyIndex(currLevel, key, startIdx, totalLen, index))
            return false;
        res[currLevel] = index;
    }
    return true;
}
KeyT StaticContBuilder::assembleKey(
            const ParsedKeyT &key)
{
    KeyT resultKey;
    size_t currLevel = 0;
    std::for_each(
        std::begin(key), std::end(key), 
        [&](size_t val)
        {
            const Key2IdxT &levelKeys = meta_[currLevel];
            const KeyT &subKey = levelKeys[val];
            if(!resultKey.empty())
                resultKey += delimeter_;
            resultKey += subKey;
            ++currLevel;
        });
    return resultKey;
}
