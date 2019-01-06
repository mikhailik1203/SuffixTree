#pragma once

#include <vector>
#include <string>
#include "SuffixTree.h"

typedef std::string KeyT;
typedef std::vector<KeyT> Key2IdxT;

class ContBuilder
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

    template<SuffixLevel>
    struct NodeTraits{
        typedef void KeyTypeT;
        typedef void ParentNodeT;
        typedef void ChildNodeT;
        typedef void NodeTypeT;
    };
    template<>
    struct NodeTraits<root_Suffix>{
        typedef std::string KeyTypeT;
        typedef void ParentNodeT;
        typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, first_Suffix> ChildNodeT;
        typedef suffix_tree::suffix_tree_impl::RootNode<ContBuilder> NodeTypeT;
    };
    template<>
    struct NodeTraits<first_Suffix>{
        typedef std::string KeyTypeT;
        typedef suffix_tree::suffix_tree_impl::RootNode<ContBuilder> ParentNodeT;
        typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, second_Suffix>  ChildNodeT;
        typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, first_Suffix> NodeTypeT;
    };
    template<>
    struct NodeTraits<second_Suffix>{
        typedef std::string KeyTypeT;
        typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, first_Suffix> ParentNodeT;
        typedef suffix_tree::suffix_tree_impl::LeafNode<ContBuilder, ValueT> ChildNodeT;
        typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, second_Suffix> NodeTypeT;
    };

    template<>
    struct NodeTraits<leaf_Suffix>{
        typedef std::string KeyTypeT;
        typedef suffix_tree::suffix_tree_impl::SuffixNode<ContBuilder, second_Suffix> ParentNodeT;
        typedef suffix_tree::suffix_tree_impl::LeafNode<ContBuilder, ValueT> NodeTypeT;

        static ValueT defaultValue(){return ValueT();}
    };

    ContBuilder(
            const Key2IdxT &lvl1, 
            const Key2IdxT &lvl2, 
            const Key2IdxT &lvl3, 
            const Key2IdxT &lvl4);
    ~ContBuilder();

    size_t levels()const;
    bool parseKey(
            const KeyT &key, 
            ContBuilder::ParsedKeyT &res)const;
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
            suffix_tree::suffix_tree_impl::IndexT &index)const;
private:
    typedef std::vector<Key2IdxT> MetaDataPerLevelsT;

    MetaDataPerLevelsT meta_;
    char delimeter_;
};
