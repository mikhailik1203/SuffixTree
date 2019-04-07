#include "ContBuilder.h"
#include <cstring>

using namespace suffix_tree;
using namespace aux;

namespace{
    Key2IndexT toKey2IndexT(const Key2IdxT &vals)
    {
        Key2IndexT res;
        std::for_each(
            std::begin(vals), std::end(vals),
            [&](const KeyT &v)
            {
                res[v.c_str()] = res.size();
            });
        return res;
    }

    std::unique_ptr<const char[]> make_unique(std::string_view str) {
        size_t count = str.size();
        std::unique_ptr<char[]> p (new char [count + 1]);
        memcpy(p.get(), str.data(), count + 1);
        p.get()[count] = 0;
        return move(p);
    }

    Key2IndexT toKey2IndexT(const Key2IdxT &vals, std::vector<std::unique_ptr<const char[]>> &storage)
    {
        Key2IndexT res;
        std::for_each(
                std::begin(vals), std::end(vals),
                [&](const KeyT &v)
                {
                    storage.emplace_back(make_unique(v));
                    res[storage.back().get()] = res.size();
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
    keyStorage_.reserve(lvl1.size() + lvl2.size() + lvl3.size() + lvl4.size());
    meta_.emplace_back(toKey2IndexT(lvl1, keyStorage_));
    meta_.emplace_back(toKey2IndexT(lvl2, keyStorage_));
    meta_.emplace_back(toKey2IndexT(lvl3, keyStorage_));
    meta_.emplace_back(toKey2IndexT(lvl4, keyStorage_));
}

ContBuilder::~ContBuilder()
{}

ContBuilder::ContBuilder(const ContBuilder &cont)
{
    delimeter_ = cont.delimeter_;
    meta_.reserve(cont.meta_.size());
    keyStorage_.reserve(cont.keyStorage_.size());
    for(auto &mit: cont.meta_)
    {
        meta_.emplace_back(Key2IndexT());
        auto &level = meta_.back();
        for(auto &it: mit)
        {
            keyStorage_.emplace_back(make_unique(it.first));
            level[keyStorage_.back().get()] = it.second;
        }
    }
}

ContBuilder& ContBuilder::operator=(ContBuilder cont)
{
    delimeter_ = cont.delimeter_;
    std::swap(keyStorage_, cont.keyStorage_);
    std::swap(meta_, cont.meta_);
    return *this;
}


size_t ContBuilder::levels()const noexcept
{
    return meta_.size();
}

size_t ContBuilder::suffixCount(
            SuffixLevel level)const noexcept
{
    assert(level < meta_.size());
    return meta_[level].size();
}

bool ContBuilder::getKeyIndex(
            size_t level, 
            const KeyT &key, 
            size_t startIdx, 
            size_t endIdx, 
            size_t &index)const
{
    const Key2IndexT &levelKeys = meta_[level];
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
    Key2IndexT &levelKeys = meta_[level];
    KeyViewT k(key.c_str() + startIdx,  endIdx - startIdx);
    auto it = levelKeys.find(k);
    if(std::end(levelKeys) != it){
        index = it->second;
        return;
    }
    index = levelKeys.size();
    keyStorage_.push_back(make_unique(k));
    levelKeys[keyStorage_.back().get()] = index;
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
