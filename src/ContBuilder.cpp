#include "ContBuilder.h"

using namespace suffix_tree;

namespace{
    Key2IndexT toKey2IndexT(const Key2IdxT &vals)
    {
        Key2IndexT res;
        size_t pos = 0;
        std::for_each(
            std::begin(vals), std::end(vals), 
            [&](const KeyT &v)
            {
                res[v.c_str()] = res.size();
            });
        return res;
    }
}

ContBuilder::ContBuilder(char delimeter):
    delimeter_(delimeter)
{
    meta_.reserve(total_Suffix);
    for(size_t i = 0; i < total_Suffix; ++i){
        meta_.push_back(Key2IndexT());
    }
}

ContBuilder::ContBuilder(
        const Key2IdxT &lvl1, 
        const Key2IdxT &lvl2, 
        const Key2IdxT &lvl3, 
        const Key2IdxT &lvl4,
        char delimeter):
    delimeter_(delimeter)
{
    meta_.reserve(4);
    meta_.push_back(toKey2IndexT(lvl1));
    meta_.push_back(toKey2IndexT(lvl2));
    meta_.push_back(toKey2IndexT(lvl3));
    meta_.push_back(toKey2IndexT(lvl4));
}

ContBuilder::~ContBuilder()
{}

size_t ContBuilder::levels()const
{
    return meta_.size();
}

size_t ContBuilder::suffixCount(
            SuffixLevel level)const
{
    return meta_[level].size();
}

bool ContBuilder::getKeyIndex(
            size_t level, 
            const KeyT &key, 
            size_t startIdx, 
            size_t endIdx, 
            suffix_tree::suffix_tree_impl::IndexT &index)const
{
    const Key2IndexT &levelKeys = meta_[level];
    std::string k(key.c_str() + startIdx,  endIdx - startIdx);
    auto it = levelKeys.find(k);
    if(std::end(levelKeys) == it)
        return false;
    index = it->second;
    return true;
}

void ContBuilder::getNewKeyIndex(
            size_t level, 
            const KeyT &key, 
            size_t startIdx, 
            size_t endIdx, 
            suffix_tree::suffix_tree_impl::IndexT &index)
{
    Key2IndexT &levelKeys = meta_[level];
    std::string k(key.c_str() + startIdx,  endIdx - startIdx);
    if(k.empty())
        levelKeys.find(k);
    auto it = levelKeys.find(k);
    if(std::end(levelKeys) != it){
        index = it->second;
        return;
    }
    index = levelKeys.size();
    levelKeys[k] = index;
}

bool ContBuilder::parseKey(
            const KeyT &key, 
            ContBuilder::ParsedKeyT &res)const
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

bool ContBuilder::parseNewKey(
            const KeyT &key, 
            ContBuilder::ParsedKeyT &res)
{
    size_t currLevel = 0;
    size_t totalLen = key.length();
    size_t tokenLastPosition[total_Suffix];
    for(size_t i = 0; i < totalLen; ++i){
        if(delimeter_ == key[i]){
            tokenLastPosition[currLevel] = i;
            ++currLevel;
            if(total_Suffix <= currLevel) /// too many tokens in key
                return false;
        }
    }
    tokenLastPosition[currLevel] = totalLen;
    if(total_Suffix != currLevel + 1)
        return false;

    size_t startIdx = 0;
    for(size_t i = 0; i < total_Suffix; ++i){
        size_t lastIdx = tokenLastPosition[i];
        suffix_tree::suffix_tree_impl::IndexT index = 0;
        getNewKeyIndex(i, key, startIdx, lastIdx, index);
        res[i] = index;
        startIdx = lastIdx + 1; ///skip delimeter
    }
    return true;
}

KeyT ContBuilder::assembleKey(
            const ParsedKeyT &key)
{
    KeyT resultKey;
    ///todo: implement it
/*    size_t currLevel = 0;
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
        });*/
    return resultKey;
}
