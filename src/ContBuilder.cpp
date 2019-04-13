#include "ContBuilder.h"
#include <cstring>

using namespace suffix_tree;
using namespace aux;

namespace{

}

ContBuilder::ContBuilder(char delimeter):
    keys_(total_Suffix),
    delimeter_(delimeter)
{}

ContBuilder::ContBuilder(
        const Key2IdxT &lvl1, 
        const Key2IdxT &lvl2, 
        const Key2IdxT &lvl3, 
        const Key2IdxT &lvl4,
        char delimeter):
    keys_(lvl1, lvl2, lvl3, lvl4),
    delimeter_(delimeter)
{}

ContBuilder::~ContBuilder()
{}

ContBuilder::ContBuilder(const ContBuilder &cont):
    keys_(total_Suffix)
{
    delimeter_ = cont.delimeter_;
    keys_ = cont.keys_;
}

ContBuilder& ContBuilder::operator=(ContBuilder cont)
{
    delimeter_ = cont.delimeter_;
    std::swap(keys_, cont.keys_);
    return *this;
}


size_t ContBuilder::levels()const noexcept
{
    return total_Suffix;
}

size_t ContBuilder::suffixCount(
            SuffixLevel level)const noexcept
{
    return keys_.suffixCount(level);
}

bool ContBuilder::getKeyIndex(
            size_t level, 
            const KeyT &key, 
            size_t startIdx, 
            size_t endIdx, 
            size_t &index)const
{
    const Key2IndexT &levelKeys = keys_.level(level);
    KeyViewT k(key.c_str() + startIdx,  endIdx - startIdx);
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
            size_t &index)
{
    const Key2IndexT &levelKeys = keys_.level(level);
    KeyViewT k(key.c_str() + startIdx,  endIdx - startIdx);
    auto it = levelKeys.find(k);
    if(std::end(levelKeys) != it){
        index = it->second;
        return;
    }
    index = keys_.addKey(level, k);
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
            size_t index = 0;
            if(!getKeyIndex(currLevel, key, startIdx, i, index))
                return false;
            res[currLevel] = index;
            ++currLevel;
            startIdx = i + 1; ///skip delimeter
        }
    }
    if(startIdx < totalLen){
        size_t index = 0;
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
    size_t lenLeft = totalLen;
    size_t tokenLastPosition[total_Suffix];
    const char *startPtr = key.c_str();
    const char *bufferPtr = startPtr;
    const char *ptr = nullptr;
    while(nullptr != (ptr = reinterpret_cast<const char *>(memchr(bufferPtr, delimeter_, lenLeft)))){
        tokenLastPosition[currLevel] = ptr - startPtr;
        ++currLevel;
        if(total_Suffix <= currLevel) /// too many tokens in key
            return false;
        lenLeft -= ptr - bufferPtr + 1;
        bufferPtr = ptr + 1;
    }

    if(total_Suffix != currLevel + 1)
        return false;
    tokenLastPosition[currLevel] = totalLen;

    size_t startIdx = 0;
    for(size_t i = 0; i < total_Suffix; ++i){
        size_t lastIdx = tokenLastPosition[i];
        size_t index = 0;
        getNewKeyIndex(i, key, startIdx, lastIdx, index);
        res[i] = index;
        startIdx = lastIdx + 1; ///skip delimeter
    }
    return true;
}

KeyT ContBuilder::assembleKey(
            const ParsedKeyT &key)const
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
