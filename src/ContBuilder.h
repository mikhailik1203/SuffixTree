#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "SuffixTree.h"
#include "ContAllocator.h"

typedef std::string KeyT;
typedef std::vector<KeyT> Key2IdxT;
typedef std::unordered_map<KeyT, size_t> Key2IndexT;

class ContBuilder
{
public:
    typedef int ValueT;
public:
    enum SuffixLevel{
        root_Suffix = 0,
        first_Suffix,
        second_Suffix,
        leaf_Suffix,
        total_Suffix
    };
    typedef size_t ParsedKeyT[total_Suffix];

    template<typename AllocatorT, SuffixLevel>
    struct NodeTraits{
        typedef void KeyTypeT;
        typedef void ParentNodeT;
        typedef void ChildNodeT;
        typedef void NodeTypeT;
    };

    explicit ContBuilder(char delimeter = '-');
    ContBuilder(
            const Key2IdxT &lvl1, 
            const Key2IdxT &lvl2, 
            const Key2IdxT &lvl3, 
            const Key2IdxT &lvl4,
            char delimeter = '-');
    ~ContBuilder();

    size_t levels()const noexcept;
    bool parseKey(
            const KeyT &key, 
            ContBuilder::ParsedKeyT &res)const;
    bool parseNewKey(
            const KeyT &key, 
            ContBuilder::ParsedKeyT &res);

    KeyT assembleKey(
            const ParsedKeyT &key);

    const Key2IdxT &keys(
            size_t level);

    size_t suffixCount(
            SuffixLevel level)const;

protected:
    bool getKeyIndex(
            size_t level, 
            const KeyT &key, 
            size_t startIdx, 
            size_t endIdx,
            size_t &index)const;

    void getNewKeyIndex(
            size_t level, 
            const KeyT &key, 
            size_t startIdx, 
            size_t endIdx,
            size_t &index);

private:
    typedef std::vector<Key2IndexT> MetaDataPerLevelsT;

    MetaDataPerLevelsT meta_;
    char delimeter_;
};

template<typename AllocatorT>
struct ContBuilder::NodeTraits<AllocatorT, ContBuilder::root_Suffix>{
    typedef std::string KeyTypeT;
    typedef void ParentNodeT;
    typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, first_Suffix, AllocatorT> ChildNodeT;
    typedef suffix_tree::suffix_tree_impl::RootNode<ContBuilder, AllocatorT> NodeTypeT;
};
template<typename AllocatorT>
struct ContBuilder::NodeTraits<AllocatorT, ContBuilder::first_Suffix>{
    typedef std::string KeyTypeT;
    typedef suffix_tree::suffix_tree_impl::RootNode<ContBuilder, AllocatorT> ParentNodeT;
    typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, second_Suffix, AllocatorT>  ChildNodeT;
    typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, first_Suffix, AllocatorT> NodeTypeT;
};
template<typename AllocatorT>
struct ContBuilder::NodeTraits<AllocatorT, ContBuilder::second_Suffix>{
    typedef std::string KeyTypeT;
    typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, first_Suffix, AllocatorT> ParentNodeT;
    typedef suffix_tree::suffix_tree_impl::LeafNode<ContBuilder, ValueT, AllocatorT> ChildNodeT;
    typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, second_Suffix, AllocatorT> NodeTypeT;
};

template<typename AllocatorT>
struct ContBuilder::NodeTraits<AllocatorT, ContBuilder::leaf_Suffix>{
    typedef std::string KeyTypeT;
    typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, second_Suffix, AllocatorT> ParentNodeT;
    typedef suffix_tree::suffix_tree_impl::LeafNode<ContBuilder, ValueT, AllocatorT> NodeTypeT;

    static ValueT defaultValue(){return ValueT();}
};

template<template<typename T, typename A, typename T::SuffixLevel LvlT> class AllocT>
class ContNodeAllocators{
    typedef ContNodeAllocators<AllocT> ThisT;
public:
    template<ContBuilder::SuffixLevel lvl>
    AllocT<ContBuilder, ThisT, lvl> &allocator()noexcept;

    typedef AllocT<ContBuilder, ThisT, ContBuilder::root_Suffix> RootNodeAllocT;
    typedef AllocT<ContBuilder, ThisT, ContBuilder::first_Suffix> FirstNodeAllocT;
    typedef AllocT<ContBuilder, ThisT, ContBuilder::second_Suffix> SecondNodeAllocT;
    typedef AllocT<ContBuilder, ThisT, ContBuilder::leaf_Suffix> LeafNodeAllocT;

    template<ContBuilder::SuffixLevel Level>
    using AllocatorT = AllocT<ContBuilder, ThisT, Level>;

private:
    RootNodeAllocT allocRoot_;
    FirstNodeAllocT allocFirst_;
    SecondNodeAllocT allocSecond_;
    LeafNodeAllocT allocLeaf_;

};

using ContNodeAllocatorsT = ContNodeAllocators<suffix_tree::suffix_tree_impl::NodeAllocator>;

template<ContBuilder::SuffixLevel Level>
using ContAllocatorsT = suffix_tree::suffix_tree_impl::NodeAllocator<ContBuilder, ContNodeAllocatorsT, Level>;

template<>template<>
inline ContAllocatorsT<ContBuilder::root_Suffix> &
ContNodeAllocators<suffix_tree::suffix_tree_impl::NodeAllocator>::allocator<ContBuilder::root_Suffix>()noexcept
{
    return allocRoot_;
}

template<>template<>
inline ContAllocatorsT<ContBuilder::first_Suffix> &
ContNodeAllocators<suffix_tree::suffix_tree_impl::NodeAllocator>::allocator<ContBuilder::first_Suffix>()noexcept
{
    return allocFirst_;
}

template<>template<>
inline ContAllocatorsT<ContBuilder::second_Suffix> &
ContNodeAllocators<suffix_tree::suffix_tree_impl::NodeAllocator>::allocator<ContBuilder::second_Suffix>()noexcept
{
    return allocSecond_;
}

template<>template<>
inline ContAllocatorsT<ContBuilder::leaf_Suffix> &
ContNodeAllocators<suffix_tree::suffix_tree_impl::NodeAllocator>::allocator<ContBuilder::leaf_Suffix>()noexcept
{
    return allocLeaf_;
}
