#pragma once

#include <vector>
#include <limits>
#include <memory>
#include <algorithm>
#include <functional>

#include "ContAllocator.h"

namespace suffix_tree{

namespace suffix_tree_impl{
    const size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

    template<typename MetaT, typename MetaT::SuffixLevel NodeLevelT, typename ContAllocatorT>
    class SuffixNode;
    template<typename MetaT, typename ValueT, typename ContAllocatorT>
    class LeafNode;

    template<typename MetaT, typename ContAllocatorT>
    class RootNode
    {
        static const typename MetaT::SuffixLevel NODE_LEVEL = MetaT::root_Suffix;
        static const typename MetaT::SuffixLevel NEXT_NODE_LEVEL = static_cast<typename MetaT::SuffixLevel>(MetaT::root_Suffix + 1);
        typedef typename MetaT::template NodeTraits<ContAllocatorT, NODE_LEVEL>::ChildNodeT ChildNodeT;
        typedef typename ContAllocatorT::template AllocatorT<NEXT_NODE_LEVEL>::NodePtrT ChildNodePtrT;
        typedef std::vector<ChildNodeT *> SubNotesT;

    public:
        RootNode(
                const MetaT &metaInfo,
                ContAllocatorT &allocator):
            metaInfo_(metaInfo), allocator_(allocator)
        {
            childNodes_.assign(metaInfo_.suffixCount(NODE_LEVEL), nullptr);
        }

        ~RootNode()
        {
            clear();
        }

        RootNode(
                const RootNode &nd,
                const MetaT &metaInfo,
                ContAllocatorT &allocator):
            metaInfo_(metaInfo),
            allocator_(allocator)
        {
            SubNotesT tmp;
            tmp.reserve(nd.childNodes_.size());  
            std::for_each(
                std::begin(nd.childNodes_), std::end(nd.childNodes_), 
                [&](const ChildNodeT *val){
                    if(nullptr != val){
                        ChildNodePtrT chld = allocator_.template allocator<NEXT_NODE_LEVEL>().
                                make_unique(*val, allocator_, this, tmp.size(), metaInfo_);
                        tmp.emplace_back(chld.get());
                        chld.release();
                    }else
                        tmp.emplace_back(nullptr);
                });
            std::swap(tmp, childNodes_);
        }

        RootNode &operator=(
                const RootNode nd)
        {
            std::swap(metaInfo_, nd.metaInfo_);
            std::swap(childNodes_, nd.childNodes_);
        }

        ChildNodeT *getChild(
                size_t index)
        {
            if(childNodes_.size() <= index)
                childNodes_.resize(index + 1, nullptr);
            if(nullptr == childNodes_[index])
                childNodes_[index] = allocator_.template allocator<NEXT_NODE_LEVEL>().
                        create(allocator_, this, index, metaInfo_);
            return childNodes_[index];
        }

        ChildNodeT *findChild(
                size_t index)noexcept
        {
            if(childNodes_.size() <= index)
                return nullptr;
            return childNodes_[index];
        }

        size_t next(
                size_t index)const noexcept
        {
            for(size_t i = index + 1; i != childNodes_.size(); ++i)
            {
                if(nullptr != childNodes_[i])
                    return i;
            }
            return INVALID_INDEX;
        }

        const ChildNodeT *begin()const noexcept
        {
            for(size_t i = 0; i != childNodes_.size(); ++i)
            {
                if(nullptr != childNodes_[i])
                    return childNodes_[i];
            }
            return nullptr;
        }

        const ChildNodeT *nextNode(
                const ChildNodeT *node)const
        {
            auto it = std::find(
                            std::begin(childNodes_), std::end(childNodes_), node);
            if(std::end(childNodes_) == it)
                return nullptr;

            ++it;
            auto nextIt = std::find_if(
                            it, std::end(childNodes_), 
                            [](const ChildNodeT *val){return nullptr != val;});
            if(std::end(childNodes_) != nextIt && nullptr != *nextIt)
                return *nextIt;
            return nullptr;
        }

        void clear()
        {
            SubNotesT tmp;
            std::swap(tmp, childNodes_);

            auto &alloc = allocator_.template allocator<NEXT_NODE_LEVEL>();
            std::for_each(std::begin(tmp), std::end(tmp),
                [&alloc](ChildNodeT *val){alloc.destroy(val);});
        }

    private:
        SubNotesT childNodes_;
        const MetaT &metaInfo_;
        ContAllocatorT &allocator_;
    };

    template<typename MetaT, typename MetaT::SuffixLevel NODE_LEVEL, typename ContAllocatorT>
    class SuffixNode
    {
        typedef typename MetaT::template NodeTraits<ContAllocatorT, NODE_LEVEL> NodeTraitsT;
        typedef typename NodeTraitsT::ParentNodeT ParentNodeT;
        typedef typename NodeTraitsT::ChildNodeT ChildNodeT;
        typedef std::vector<ChildNodeT *> SubNotesT;

        static const typename MetaT::SuffixLevel NEXT_NODE_LEVEL = static_cast<typename MetaT::SuffixLevel>(NODE_LEVEL + 1);
        typedef typename ContAllocatorT::template AllocatorT<NEXT_NODE_LEVEL>::NodePtrT ChildNodePtrT;

    public:
        SuffixNode(
                ContAllocatorT &allocator,
                ParentNodeT *parentNode,
                size_t index,
                const MetaT &metaInfo):
            allocator_(allocator), metaInfo_(metaInfo),
            parentNode_(parentNode), selfIndex_(index)
        {
            childNodes_.assign(metaInfo_.suffixCount(NODE_LEVEL), nullptr);    
        }

        SuffixNode(
                const SuffixNode &nd,
                ContAllocatorT &allocator,
                ParentNodeT *parentNode,
                size_t index,
                const MetaT &metaInfo):
            allocator_(allocator), metaInfo_(metaInfo),
            parentNode_(parentNode), selfIndex_(index)
        {
            SubNotesT tmp;
            tmp.reserve(nd.childNodes_.size());
            std::for_each(
                    std::begin(nd.childNodes_), std::end(nd.childNodes_),
                    [&](const ChildNodeT *val){
                        if(nullptr != val){
                            ChildNodePtrT chld = allocator_.template allocator<NEXT_NODE_LEVEL>().
                                    make_unique(*val, allocator_, this, tmp.size(), metaInfo_);
                            tmp.emplace_back(chld.get());
                            chld.release();
                        }else
                            tmp.emplace_back(nullptr);
                    });
            std::swap(tmp, childNodes_);
            selfIndex_ = nd.selfIndex_;
        }

        ~SuffixNode()
        {
            clear();
        }

        SuffixNode(const SuffixNode &nd) = delete;
        SuffixNode &operator=(const SuffixNode nd) = delete;

        ChildNodeT *getChild(
                size_t index)
        {
            if(childNodes_.size() <= index)
                childNodes_.resize(index + 1, nullptr);

            if(nullptr == childNodes_[index])
                childNodes_[index] = allocator_.template allocator<NEXT_NODE_LEVEL>().
                        create(allocator_, this, index, metaInfo_);

            return childNodes_[index];
        }

        ChildNodeT *findChild(
                size_t index)const noexcept
        {
            if(childNodes_.size() < index)
                return nullptr;

            return childNodes_[index];
        }

        size_t next(
                size_t index)const noexcept
        {
            for(size_t i = index + 1; i != childNodes_.size(); ++i)
            {
                if(nullptr != childNodes_[i])
                    return i;
            }
            return INVALID_INDEX;
        }

        const ChildNodeT *begin()const
        {
            auto it = std::find_if(
                        std::begin(childNodes_), std::end(childNodes_), 
                        [](const ChildNodeT * val){return nullptr != val;});
            if(std::end(childNodes_) == it)
                return nullptr;
            return *it;
        }

        const ChildNodeT *nextNode(
                const ChildNodeT *node)const
        {
            typename SubNotesT::const_iterator it = std::find(
                            std::begin(childNodes_), std::end(childNodes_), node);
            if(std::end(childNodes_) == it)
                return nullptr;

            ++it;
            auto nextIt = std::find_if(
                            it, childNodes_.end(), 
                            [](const ChildNodeT *val){return nullptr != val;});
            if(childNodes_.end() != nextIt && nullptr != *nextIt)
                return *nextIt;

            auto nextNode = parentNode_->nextNode(this);
            if(nullptr != nextNode)
                return nextNode->begin();
            return nullptr;
        }

        void clear()
        {
            SubNotesT tmp(metaInfo_.suffixCount(NODE_LEVEL), nullptr);
            std::swap(tmp, childNodes_);

            auto &alloc = allocator_.template allocator<NEXT_NODE_LEVEL>();
            std::for_each(
                std::begin(tmp), std::end(tmp), 
                [&alloc](ChildNodeT *val){alloc.destroy(val);});
        }

    private:
        const MetaT &metaInfo_;
        ContAllocatorT &allocator_;
        SubNotesT childNodes_;
        ParentNodeT *parentNode_;
        size_t selfIndex_;
    };

    template<typename MetaT, typename ValueT, typename ContAllocatorT>
    class LeafNode
    {
        typedef typename MetaT::template NodeTraits<ContAllocatorT, MetaT::leaf_Suffix> NodeTraitsT;
        typedef typename NodeTraitsT::ParentNodeT ParentNodeT;
        typedef std::vector<ValueT> ValuesT;

        static const bool VALUE_EXIST = true;
        static const bool VALUE_MISSED = false;
        typedef std::vector<bool> ValueOptionalT;

    public:
        LeafNode(
                ContAllocatorT &allocator,
                ParentNodeT *parentNode,
                size_t index,
                const MetaT &metaInfo): 
            parentNode_(parentNode), selfIndex_(index) 
        {
            size_t count = metaInfo.suffixCount(MetaT::leaf_Suffix);
            values_.resize(count, NodeTraitsT::defaultValue());
            optional_.resize(count, VALUE_MISSED);
        }

        LeafNode(
                const LeafNode &nd,
                ContAllocatorT &allocator,
                ParentNodeT *parentNode,
                size_t index,
                const MetaT &metaInfo):
            parentNode_(parentNode), selfIndex_(index)
        {
            ValuesT tmp;
            tmp.reserve(nd.values_.size());
            ValueOptionalT tmpOptional(nd.optional_);
            std::for_each(
                    std::begin(nd.values_), std::end(nd.values_),
                    [&](const ValueT &val){
                        tmp.push_back(val);
                    });
            std::swap(tmp, values_);
            std::swap(tmpOptional, optional_);
            selfIndex_ = nd.selfIndex_;
        }

        ~LeafNode(){}

        LeafNode(const LeafNode &nd) = delete;
        LeafNode &operator=(const LeafNode nd) = delete;

        const ValuesT &values()const noexcept{return values_;}

        bool set(
                size_t index,
                const ValueT &val)
        {
            if(values_.size() <= index){
                values_.resize(index + 1, NodeTraitsT::defaultValue());
                optional_.resize(index + 1, false);
            }

            values_[index] = val;
            if(VALUE_EXIST == optional_[index])
                return false;
            optional_[index] = VALUE_EXIST;
            return true;
        }

        ValueT &get(
                size_t index)const
        {
            if((values_.size() <= index) || (VALUE_MISSED == optional_[index]))
                throw std::runtime_error("LeafNode::get: element is not exist at index");
            return values_[index];
        }

        bool exist(
                size_t index)const noexcept
        {
            return (optional_.size() > index) && (VALUE_EXIST == optional_[index]);
        }

        bool erase(
                size_t index)
        {
            if ((optional_.size() <= index) || (VALUE_MISSED == optional_[index]))
                return false;
            values_[index] = NodeTraitsT::defaultValue();
            optional_[index] = VALUE_MISSED;
            return true;
        }

        size_t next(
                size_t index)const noexcept
        {
            for(size_t i = index + 1; i != optional_.size(); ++i)
            {
                if(VALUE_EXIST == optional_[i])
                    return i;
            }
            return INVALID_INDEX;
        }

        size_t begin()const noexcept
        {
            for(size_t i = 0; i != optional_.size(); ++i)
            {
                if(VALUE_EXIST == optional_[i])
                    return i;
            }
            return INVALID_INDEX;
        }


        void clear()
        {
            size_t count = optional_.size();
            values_.clear();
            values_.resize(count, ValueT());
            optional_.clear();
            optional_.resize(count, VALUE_MISSED);
        }

        const ParentNodeT *parent()const noexcept{return parentNode_;}

    private:
        ParentNodeT *parentNode_;
        mutable ValuesT values_;
        ValueOptionalT optional_;
        size_t selfIndex_;
    };

}

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

    ~SuffixTreeIterator()
    {}

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
        auto *nextNode = prntNode->nextNode(node_);
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

template<class ContBuilderT,
         typename KeyT,
         typename ContValueT,
         typename ContAllocatorT>
class SuffixTree
{
public:
    typedef ContBuilderT BuilderT;
    typedef ContValueT ValueT;
    typedef SuffixTree<ContBuilderT, KeyT, ContValueT, ContAllocatorT> ThisTypeT;
    typedef SuffixTreeIterator<ThisTypeT> Iterator;
    typedef typename ContBuilderT::template NodeTraits<ContAllocatorT, ContBuilderT::leaf_Suffix>::NodeTypeT LeafNodeT;
    typedef std::function<Iterator(LeafNodeT *node)> NodeFunctorT;
    typedef std::function<Iterator(const LeafNodeT *node)> CNodeFunctorT;
    typedef suffix_tree_impl::RootNode<BuilderT, ContAllocatorT> RootNodeT;
    typedef typename ContAllocatorT::RootNodeAllocT::NodePtrT RootNodePtrT;

public:
    explicit SuffixTree(
            const BuilderT &builder):
        builder_(builder),
        allocator_(),
        root_(allocator_.template allocator<ContBuilderT::root_Suffix>().
              make_unique(builder_, allocator_)),
        size_(0)
    {
    }

    ~SuffixTree()
    {
        clear();
    }

    SuffixTree(
            const SuffixTree &sft):
        builder_(sft.builder_),
        allocator_(),
        root_(allocator_.template allocator<ContBuilderT::root_Suffix>().
              make_unique(*sft.root_, builder_, allocator_)),
        size_(sft.size_)
    {}

    SuffixTree &operator=(
            SuffixTree sft)
    {
        std::swap(builder_, sft.builder_);
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
        typename ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseNewKey(key, parsedKey))
            return end();
        size_t index = 0;

        auto insertFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                if(node->set(parsedKey[index], val))
                    ++size_;
                return ThisTypeT::Iterator(node, parsedKey[index]);
            };

        return applyFunc(root_.get(), parsedKey, index, insertFunc);
    }

    Iterator find(const KeyT &key)const
    {
        typename ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseKey(key, parsedKey))
            return end();
        size_t index = 0;

        auto findFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                if(!node->exist(parsedKey[index]))
                    return end();
                return ThisTypeT::Iterator(node, parsedKey[index]);
            };

        return applyFunc(root_.get(), parsedKey, index, findFunc);
    }

    Iterator erase(const KeyT &key)
    {
        typename ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseKey(key, parsedKey))
            return end();
        size_t index = 0;

        auto eraseFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                auto nextIt = ThisTypeT::Iterator(node, parsedKey[index]).next();
                if(node->erase(parsedKey[index]))
                    --size_;
                return nextIt;
            };
        ///todo: erase parent node, if last child node was erased
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
                const typename ContBuilderT::ParsedKeyT &key, 
                size_t &index, 
                NodeFunctorT func)const
    {
        auto *childNode = node->getChild(key[index++]);
        if(nullptr == childNode)
            return end();
        return applyFunc(childNode, key, index, func);
    }

    Iterator applyFunc(
                suffix_tree_impl::SuffixNode<BuilderT, static_cast<typename BuilderT::SuffixLevel>(BuilderT::leaf_Suffix - 1), ContAllocatorT> *node,
                const typename ContBuilderT::ParsedKeyT &key, 
                size_t &index, 
                NodeFunctorT func)const
    {
        auto *childNode = node->getChild(key[index++]);
        if(nullptr == childNode)
            return end();
        return func(childNode);
    }

    template<typename NodeT>
    Iterator applyFunc(
                const NodeT *node, 
                CNodeFunctorT func)const
    {
        auto *childNode = node->begin();
        if(nullptr == childNode)
            return end();
        return applyFunc(childNode, func);
    }

    Iterator applyFunc(
                const suffix_tree_impl::SuffixNode<BuilderT,
                        static_cast<typename BuilderT::SuffixLevel>(BuilderT::leaf_Suffix - 1), ContAllocatorT> *node,
                CNodeFunctorT func)const
    {
        const auto *childNode = node->begin();
        if(nullptr == childNode)
            return end();
        return func(childNode);
    }

private:
    BuilderT builder_;
    ContAllocatorT allocator_;

    RootNodePtrT root_;
    size_t size_;
};


}


