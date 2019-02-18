#include "StaticContBuilder.h"
#include <cstring>

using namespace st_suffix_tree;

namespace{
    void makeDeepCopy(MetaDataPerLevelsT &meta, const Key2IdxT &vals)
    {
        meta.push_back(Key2IdxT());
        Key2IdxT &mVal = meta.back();
        mVal.reserve(vals.size());
        for(auto &v: vals){
            mVal.push_back(v.c_str());
        }
    }

}

StaticContBuilder::StaticContBuilder(
        const Key2IdxT &lvl1,
        const Key2IdxT &lvl2, 
        const Key2IdxT &lvl3, 
        const Key2IdxT &lvl4,
        char delimeter):
    delimeter_(delimeter)
{
    meta_.reserve(4);
#ifdef MEM_USAGE_TEST_
    makeDeepCopy(meta_, lvl1);
    makeDeepCopy(meta_, lvl2);
    makeDeepCopy(meta_, lvl3);
    makeDeepCopy(meta_, lvl4);
#else
    meta_.push_back(lvl1);
    meta_.push_back(lvl2);
    meta_.push_back(lvl3);
    meta_.push_back(lvl4);
#endif
    std::sort(std::begin(meta_[0]), std::end(meta_[0]));
    std::sort(std::begin(meta_[1]), std::end(meta_[1]));
    std::sort(std::begin(meta_[2]), std::end(meta_[2]));
    std::sort(std::begin(meta_[3]), std::end(meta_[3]));
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
            st_suffix_tree::st_suffix_tree_impl::IndexT &index)const
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
            st_suffix_tree::st_suffix_tree_impl::IndexT index = 0;
            if(!getKeyIndex(currLevel, key, startIdx, i, index))
                return false;
            res[currLevel] = index;
            ++currLevel;
            startIdx = i + 1; ///skip delimeter
        }
    }
    if(startIdx < totalLen){
        st_suffix_tree::st_suffix_tree_impl::IndexT index = 0;
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
