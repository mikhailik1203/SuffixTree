//
// Created by sam1203 on 6/7/19.
//
#pragma once

#include <memory>
#include <functional>

#include "SuffixTreeImpl.h"

namespace suffix_tree{

    template<typename ContT>
    class SuffixTreeIterator : public std::iterator<std::forward_iterator_tag, typename ContT::ValueT>
    {
        typedef typename ContT::ValueT ValueT;
        typedef typename ContT::LeafNodeT LeafNodeT;

        friend ContT;
    public:
        SuffixTreeIterator():
                node_(nullptr), index_(suffix_tree_impl::INVALID_INDEX)
        {}

        SuffixTreeIterator(
                const SuffixTreeIterator& it):
                node_(it.node_), index_(it.index_)
        {}

        SuffixTreeIterator &operator=(
                SuffixTreeIterator it)
        {
            std::swap(it.index_, this->index_);
            std::swap(it.node_, this->node_);
            return *this;
        }

        ~SuffixTreeIterator() = default;

        ValueT& operator*(){
            if(nullptr == node_)
                throw std::runtime_error("SuffixTreeIterator::op*: Invalid level at SuffixTreeIterator!");
            return node_->get(index_);
        }

        ValueT value()const
        {
            if(nullptr == node_)
                throw std::runtime_error("SuffixTreeIterator::value: Invalid level at SuffixTreeIterator!");
            return node_->get(index_);
        }

        bool operator==(
                const SuffixTreeIterator &val)const noexcept
        {
            return node_ == val.node_ && index_ == val.index_;
        }
        bool operator!=(
                const SuffixTreeIterator &val)const noexcept
        {
            return node_ != val.node_ || index_ != val.index_;
        }

        SuffixTreeIterator next()const
        {
            if(nullptr == node_ || suffix_tree_impl::INVALID_INDEX == index_)
                return SuffixTreeIterator();
            size_t nextIdx = node_->next(index_);
            if(suffix_tree_impl::INVALID_INDEX != nextIdx)
                return SuffixTreeIterator(node_, nextIdx);

            auto *prntNode = node_->parent();
            if(nullptr == prntNode)
                return SuffixTreeIterator();
            auto * nextNode = prntNode->nextNode(node_);
            if(nullptr == nextNode)
                return SuffixTreeIterator();
            return SuffixTreeIterator(nextNode, nextNode->begin());
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
        SuffixTreeIterator(
                const LeafNodeT *level,
                size_t index):
                node_(level), index_(index)
        {
            if(suffix_tree_impl::INVALID_INDEX == index_)
                node_ = nullptr;
        }

        const LeafNodeT *node()const noexcept{return node_;}

        size_t index()const noexcept{return index_;}

    private:
        const LeafNodeT *node_;
        size_t index_;
    };

    template<class ContTraitsT>
    class SuffixTree
    {
    public:
        typedef ContTraitsT TraitsT;
        typedef typename TraitsT::KeyT KeyT;
        typedef typename TraitsT::ValueT ValueT;
        typedef SuffixTree<ContTraitsT> ThisTypeT;
        typedef SuffixTreeIterator<ThisTypeT> Iterator;
        typedef typename ContTraitsT::template NodeTraits<ContTraitsT::SuffixLevel::leaf_Suffix, void>::NodeTypeT LeafNodeT;
        typedef std::function<Iterator(LeafNodeT *node)> NodeFunctorT;
        typedef std::function<Iterator(const LeafNodeT *node)> CNodeFunctorT;
        typedef suffix_tree_impl::RootNode<TraitsT> RootNodeT;
        typedef std::unique_ptr<RootNodeT> RootNodePtrT;

    public:
        explicit SuffixTree(
                const TraitsT &traits):
                traits_(traits),
                root_(new RootNodeT(traits_)),
                size_(0)
        {
        }

        ~SuffixTree()
        {
            clear();
        }

        SuffixTree(
                const SuffixTree &sft):
                traits_(sft.traits_),
                root_(new RootNodeT(*sft.root_, traits_)),
                size_(sft.size_)
        {}

        SuffixTree &operator=(
                SuffixTree sft)
        {
            std::swap(traits_, sft.traits_);
            root_ = sft.root_;
            size_ = sft.size_;
        }

        Iterator begin()const
        {
            auto findFunc = [&, this](const LeafNodeT *node)->ThisTypeT::Iterator
            {
                size_t idx = node->begin();
                if(suffix_tree_impl::INVALID_INDEX == idx)
                    return end();
                return ThisTypeT::Iterator(node, idx);
            };

            return applyFunc(root_.get(), findFunc);
        }

        Iterator end()const noexcept
        {
            return Iterator();
        }

        Iterator insert(
                const KeyT &key,
                const ValueT &val)
        {
            typename ContTraitsT::ParsedKeyT parsedKey;
            if(!traits_.parseNewKey(key, parsedKey))
                return end();
            size_t index = 0;
            size_t leafIndex = parsedKey[ContTraitsT::SuffixLevel::leaf_Suffix];
            auto insertFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                if(node->set(leafIndex, val))
                    ++size_;
                return ThisTypeT::Iterator(node, leafIndex);
            };

            return applyFunc(root_.get(), parsedKey, index, insertFunc);
        }

        Iterator find(const KeyT &key)const
        {
            typename ContTraitsT::ParsedKeyT parsedKey;
            if(!traits_.parseKey(key, parsedKey))
                return end();
            size_t index = 0;
            size_t leafIndex = parsedKey[ContTraitsT::SuffixLevel::leaf_Suffix];
            auto findFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                if(!node->exist(leafIndex))
                    return end();
                return ThisTypeT::Iterator(node, leafIndex);
            };

            return applyFunc(root_.get(), parsedKey, index, findFunc);
        }

        Iterator erase(const KeyT &key)
        {
            typename ContTraitsT::ParsedKeyT parsedKey;
            if(!traits_.parseKey(key, parsedKey))
                return end();
            size_t index = 0;
            size_t leafIndex = parsedKey[ContTraitsT::SuffixLevel::leaf_Suffix];
            auto eraseFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                auto nextIt = ThisTypeT::Iterator(node, leafIndex).next();
                if(node->erase(leafIndex))
                    --size_;
                return nextIt;
            };
            return applyFunc(root_.get(), parsedKey, index, eraseFunc);
        }

        Iterator erase(const Iterator &it)
        {
            if(end() == it)
                return end();
            Iterator nextIt = it.next();
            if(const_cast<typename Iterator::LeafNodeT *>(it.node())->erase(it.index()))
                --size_;
            return nextIt;
        }

        size_t size()const noexcept{return size_;}

        void clear()
        {
            size_ = 0;
            root_->clear();
        }

    private:
        template<typename NodeT>
        Iterator applyFunc(
                NodeT *node,
                const typename ContTraitsT::ParsedKeyT &key,
                size_t &index,
                NodeFunctorT func)const
        {
            auto *childNode = node->getChild(key[index++]);
            if(nullptr == childNode)
                return end();
            return applyFunc(childNode, key, index, func);
        }

        Iterator applyFunc(
                LeafNodeT *node,
                const typename ContTraitsT::ParsedKeyT &key,
                size_t &index,
                NodeFunctorT func)const
        {
            return func(node);
        }

        template<typename NodeT>
        Iterator applyFunc(
                const NodeT *node,
                CNodeFunctorT func)const
        {
            auto * childNode = node->begin();
            if(nullptr == childNode)
                return end();
            return applyFunc(childNode, func);
        }

        Iterator applyFunc(
                const LeafNodeT *node,
                CNodeFunctorT func)const
        {
            return func(node);
        }

    private:
        TraitsT traits_;

        RootNodePtrT root_;
        size_t size_;
    };


}

