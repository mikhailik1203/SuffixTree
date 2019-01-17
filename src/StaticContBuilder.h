#pragma once

#include <vector>
#include <string>
#include "SuffixTree.h"

typedef std::string KeyT;
typedef std::vector<KeyT> Key2IdxT;

class StaticContBuilder
{
public:
    typedef int ValueT;
public:
    enum SuffixLevel{
        root_Suffix = 0,
        first_Suffix,
        second_Suffix,
        leaf_Suffix
    };
    typedef suffix_tree::suffix_tree_impl::IndexT ParsedKeyT[leaf_Suffix + 1];

    StaticContBuilder(
            const Key2IdxT &lvl1, 
            const Key2IdxT &lvl2, 
            const Key2IdxT &lvl3, 
            const Key2IdxT &lvl4);
    ~StaticContBuilder();

    bool parseKey(
            const KeyT &key, 
            StaticContBuilder::ParsedKeyT &res)const;
    KeyT assembleKey(
            const ParsedKeyT &key);

    size_t suffixCount(
            SuffixLevel level)const;

    static ValueT defaultValue(){return ValueT();}

protected:
    bool getKeyIndex(
            size_t level, 
            const KeyT &key, 
            size_t startIdx, 
            size_t endIdx, 
            suffix_tree::suffix_tree_impl::IndexT &index)const;
private:
    typedef std::vector<Key2IdxT> MetaDataPerLevelsT;

    MetaDataPerLevelsT meta_;
    char delimeter_;
};
