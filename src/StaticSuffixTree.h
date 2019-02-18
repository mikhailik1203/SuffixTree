#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include <functional>

namespace st_suffix_tree{

namespace st_suffix_tree_impl{
    typedef size_t IndexT;
    const IndexT INVALID_INDEX = std::numeric_limits<size_t>::max();
}

template<typename ContT>
class SuffixTreeIterator : public std::iterator<std::forward_iterator_tag, typename ContT::ValueT>
{
    typedef typename ContT::ValueT ValueT;
    
    friend ContT;
public:
    SuffixTreeIterator(): 
        cont_(nullptr), index_(st_suffix_tree_impl::INVALID_INDEX)
    {}

    SuffixTreeIterator(
            const SuffixTreeIterator& it): 
        cont_(it.cont_), index_(it.index_)
    {}

    SuffixTreeIterator &operator=(
            SuffixTreeIterator it)
    {
        std::swap(it.cont_, this->cont_);
        std::swap(it.index_, this->index_);
        return *this;
    }

    ~SuffixTreeIterator()
    {}

    ValueT& operator*(){
        if(nullptr == cont_)
            throw std::runtime_error("SuffixTreeIterator::op*: Invalid SuffixTreeIterator!");
        return cont_->get(index_);
    }

    ValueT value()const
    {
        if(nullptr == cont_)
            throw std::runtime_error("SuffixTreeIterator::value: Invalid SuffixTreeIterator!");
        return cont_->get(index_);
    }

    bool operator==(
            const SuffixTreeIterator &val)const
    {
        return cont_ == val.cont_ && index_ == val.index_;
    }
    bool operator!=(
            const SuffixTreeIterator &val)const
    {
        return cont_ != val.cont_ || index_ != val.index_;
    }
    SuffixTreeIterator next()const
    {
        if(nullptr == cont_ || st_suffix_tree_impl::INVALID_INDEX == index_)
            return SuffixTreeIterator();
        return cont_->next(index_);
    }

    SuffixTreeIterator& operator++() 
    {
        next();
        return *this;
    }

    SuffixTreeIterator operator++(int)
    {
        SuffixTreeIterator tmp(*this); 
        operator++(); 
        return tmp;
    }

protected:
    st_suffix_tree_impl::IndexT index()const{return index_;}

protected:
    SuffixTreeIterator(
            const ContT *cont, 
            st_suffix_tree_impl::IndexT index): 
        cont_(cont), index_(index)
    {
        if(st_suffix_tree_impl::INVALID_INDEX == index_)
            cont_ = nullptr;
    }

private:
    const ContT *cont_;
    st_suffix_tree_impl::IndexT index_;
};

template<typename ContBuilderT, typename KeyT, typename ContValueT>
class StaticSuffixTree
{
public:
    typedef ContBuilderT BuilderT;
    typedef ContValueT ValueT;
    typedef StaticSuffixTree<ContBuilderT, KeyT, ContValueT> ThisTypeT;
    typedef SuffixTreeIterator<ThisTypeT> Iterator;

    friend Iterator;
public:
    StaticSuffixTree(
            const BuilderT &builder):
        builder_(builder), size_(0)
    {
        size_t totalSize = 1;
        for(size_t lvl = BuilderT::root_Suffix; lvl <= BuilderT::leaf_Suffix; ++lvl)
        {
            totalSize *= builder.suffixCount(static_cast<typename BuilderT::SuffixLevel>(lvl));
        }
        optional_.assign(totalSize, 0);
        values_.assign(totalSize, BuilderT::defaultValue());
    }

    ~StaticSuffixTree()
    {
        clear();
    }

    StaticSuffixTree(
            const StaticSuffixTree &sft):
        builder_(sft.builder_), values_(sft.values_), optional_(sft.optional_), size_(sft.size_)
    {}

    StaticSuffixTree &operator=(
            const StaticSuffixTree sft)
    {
        std::swap(builder_, sft.builder_);
        std::swap(values_, sft.values_);
        std::swap(optional_, sft.optional_);
        size_ = sft.size_;
    }

    Iterator begin()const
    {
        for(size_t i = 0; i < optional_.size(); ++i)
            if(optional_[i])
                return Iterator(this, i);
        return end();
    }

    Iterator end()const
    {
        return Iterator();
    }

    Iterator insert(
            const KeyT &key, 
            const ValueT &val)
    {
        typename ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseKey(key, parsedKey))
            return end();

        size_t index = calcIndex(parsedKey);
        values_[index] = val;
        if(!optional_[index])
            ++size_;
        optional_[index] = true;
        return Iterator(this, index);
    }

    Iterator find(const KeyT &key)const
    {
        typename ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseKey(key, parsedKey))
            return end();
        size_t index = calcIndex(parsedKey);
        if(optional_[index])
            return Iterator(this, index);
        return end();
    }

    Iterator erase(const KeyT &key)
    {
        typename ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseKey(key, parsedKey))
            return end();
        size_t index = calcIndex(parsedKey);
        if(!optional_[index])
            return end();
        optional_[index] = false;
        --size_;
        return next(index);
    }

    Iterator erase(const Iterator &it)
    {
        if(end() == it)
            return end();
        Iterator nextIt = it.next();
        size_t index = it.index();
        if(optional_[index]){
            --size_;
            optional_[index] = false;
            return next(index);
        }
        return end();
    }

    size_t size()const{return size_;}

    void clear()
    {
        size_ = 0;
        size_t s = optional_.size();
        optional_.assign(s, 0);
        values_.assign(s, BuilderT::defaultValue());
    }

private:

    Iterator next(st_suffix_tree_impl::IndexT index)const
    {
        for(size_t i = index + 1; i < optional_.size(); ++i)
            if(optional_[i])
                return Iterator(this, i);
        return end();
    }


    ValueT &get(
            st_suffix_tree_impl::IndexT index)const
    {
        if(!optional_[index])
            throw std::runtime_error("StaticSuffixTree::get: element is not exist at index");
        return values_[index];
    }

    size_t calcIndex(const typename ContBuilderT::ParsedKeyT &key)const
    {
        size_t index = key[BuilderT::root_Suffix];
        for(size_t lvl = BuilderT::root_Suffix + 1; lvl <= BuilderT::leaf_Suffix; ++lvl)
        {
            index = index*builder_.suffixCount(static_cast<typename BuilderT::SuffixLevel>(lvl)) + key[lvl];
        }
        return index;
    }

private:
    BuilderT builder_;
    mutable std::vector<ValueT> values_;
    std::vector<bool> optional_;
    size_t size_;
};


}


