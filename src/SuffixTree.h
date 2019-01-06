#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include <functional>

namespace suffix_tree{

namespace suffix_tree_impl{
    typedef size_t IndexT;
    const IndexT INVALID_INDEX = std::numeric_limits<size_t>::max();

    template<typename MetaT, typename MetaT::SuffixLevel NodeLevelT>
    class SuffixNode;
    template<typename MetaT, typename ValueT>
    class LeafNode;

    template<typename MetaT>
    class RootNode
    {
        static const typename MetaT::SuffixLevel NODE_LEVEL = MetaT::root_Suffix;
        typedef typename MetaT::NodeTraits<NODE_LEVEL>::ChildNodeT ChildNodeT;
        typedef std::vector<ChildNodeT *> SubNotesT;
    public:
        RootNode(
                const MetaT &metaInfo): 
            metaInfo_(metaInfo)
        {
            childNodes_.assign(metaInfo_.suffixCount(NODE_LEVEL), nullptr);
        }
        ~RootNode()
        {
            clear();
        }


        ChildNodeT *getChild(
                IndexT index)
        {
            if(nullptr == childNodes_[index])
                childNodes_[index] = new ChildNodeT(this, metaInfo_);
            return childNodes_[index];
        }

        ChildNodeT *findChild(
                IndexT index)
        {
            return childNodes_[index];
        }

        IndexT next(
                IndexT index)
        {
            for(size_t i = index + 1; i != childNodes_.size(); ++i)
            {
                if(nullptr != childNodes_[i])
                    return i;
            }
            return INVALID_INDEX;
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
            std::for_each(std::begin(childNodes_), std::end(childNodes_), 
                [](ChildNodeT *val){delete val;});
            childNodes_.clear();
            childNodes_.assign(metaInfo_.suffixCount(NODE_LEVEL), nullptr);
        }

    private:
        SubNotesT childNodes_;
        const MetaT &metaInfo_;
    };

    template<typename MetaT, typename MetaT::SuffixLevel NODE_LEVEL>
    class SuffixNode
    {
        typedef typename MetaT::NodeTraits<NODE_LEVEL> NodeTraitsT;
        typedef typename NodeTraitsT::ParentNodeT ParentNodeT;
        typedef typename NodeTraitsT::ChildNodeT ChildNodeT;
        typedef std::vector<ChildNodeT *> SubNotesT;
    public:
        SuffixNode(
                ParentNodeT *parentNode, 
                const MetaT &metaInfo): 
            metaInfo_(metaInfo), parentNode_(parentNode)
        {
            childNodes_.assign(metaInfo_.suffixCount(NODE_LEVEL), nullptr);    
        }
        ~SuffixNode()
        {
            clear();
        }

        ChildNodeT *getChild(
                IndexT index)
        {
            if(nullptr == childNodes_[index])
                childNodes_[index] = new ChildNodeT(this, metaInfo_);
            return childNodes_[index];
        }

        ChildNodeT *findChild(
                IndexT index)const
        {
            return childNodes_[index];
        }

        IndexT next(
                IndexT index)const
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
            SubNotesT::const_iterator it = std::find(
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
            std::for_each(
                std::begin(childNodes_), std::end(childNodes_), 
                [](ChildNodeT *val){delete val;});
            childNodes_.clear();
            childNodes_.assign(metaInfo_.suffixCount(NODE_LEVEL), nullptr);    
        }

    private:
        SubNotesT childNodes_;
        ParentNodeT *parentNode_;
        const MetaT &metaInfo_;
    };

    template<typename MetaT, typename ValueT>
    class LeafNode
    {
        typedef typename MetaT::NodeTraits<MetaT::leaf_Suffix> NodeTraitsT;
        typedef typename NodeTraitsT::ParentNodeT ParentNodeT;
        typedef std::vector<ValueT> ValuesT;

        static const char VALUE_EXIST = 'Y';
        static const char VALUE_MISSED = 'N';

    public:
        LeafNode(
                ParentNodeT *parentNode, 
                const MetaT &metaInfo): 
            parentNode_(parentNode) 
        {
            size_t count = metaInfo.suffixCount(MetaT::leaf_Suffix);
            values_.resize(count, NodeTraitsT::defaultValue());
            optional_.resize(count, VALUE_MISSED);
        }
        ~LeafNode(){}

        const ValuesT &values()const{return values_;}

        bool set(
                IndexT index, 
                const ValueT &val)
        {
            values_[index] = val;
            if(VALUE_EXIST == optional_[index])
                return false;
            optional_[index] = VALUE_EXIST;
            return true;
        }

        ValueT &get(
                IndexT index)const
        {
            if(VALUE_MISSED == optional_[index])
                throw std::runtime_error("LeafNode::get: element is not exist at index");
            return values_[index];
        }

        bool exist(
                IndexT index)const
        {
            return (VALUE_EXIST == optional_[index]);
        }

        bool erase(
                IndexT index)
        {
            if(VALUE_MISSED == optional_[index])
                return false;
            values_[index] = NodeTraitsT::defaultValue();
            optional_[index] = VALUE_MISSED;
            return true;
        }

        IndexT next(
                IndexT index)const
        {
            for(size_t i = index + 1; i != optional_.size(); ++i)
            {
                if(VALUE_EXIST == optional_[i])
                    return i;
            }
            return INVALID_INDEX;
        }

        IndexT begin()const
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


        const ParentNodeT *parent()const{return parentNode_;}
    private:
        ParentNodeT *parentNode_;
        mutable ValuesT values_;
        std::vector<char> optional_;
    };

}

template<typename ContT>
class SuffixTreeIterator : public std::iterator<std::forward_iterator_tag, typename ContT::ValueT>
{
    typedef typename ContT::ValueT ValueT;
    typedef typename ContT::BuilderT::NodeTraits<ContT::BuilderT::leaf_Suffix>::NodeTypeT LeafNodeT;
    
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
            const SuffixTreeIterator &val)const
    {
        return node_ == val.node_ && index_ == val.index_;
    }
    bool operator!=(
            const SuffixTreeIterator &val)const
    {
        return node_ != val.node_ || index_ != val.index_;
    }
    SuffixTreeIterator next()const
    {
        if(nullptr == node_ || suffix_tree_impl::INVALID_INDEX == index_)
            return SuffixTreeIterator();
        suffix_tree_impl::IndexT nextIdx = node_->next(index_);
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
    const LeafNodeT *node()const{return node_;}
    suffix_tree_impl::IndexT index()const{return index_;}

protected:
    SuffixTreeIterator(
            const LeafNodeT *level, 
            suffix_tree_impl::IndexT index): 
        node_(level), index_(index)
    {
        if(suffix_tree_impl::INVALID_INDEX == index_)
            node_ = nullptr;
    }

private:
    const LeafNodeT *node_;
    suffix_tree_impl::IndexT index_;
};

template<typename ContBuilderT, typename KeyT, typename ContValueT>
class SuffixTree
{
public:
    typedef ContBuilderT BuilderT;
    typedef ContValueT ValueT;
    typedef SuffixTree<ContBuilderT, KeyT, ContValueT> ThisTypeT;
    typedef typename SuffixTreeIterator<ThisTypeT> Iterator;
    typedef typename ContBuilderT::NodeTraits<ContBuilderT::leaf_Suffix>::NodeTypeT LeafNodeT;
    typedef std::function<Iterator(LeafNodeT *node)> NodeFunctorT;
    typedef suffix_tree_impl::RootNode<BuilderT> RootNodeT;

public:
    SuffixTree(
            const BuilderT &builder):
        builder_(builder), root_(new suffix_tree_impl::RootNode<BuilderT>(builder)), size_(0)
    {}

    ~SuffixTree()
    {
        clear();
    }


    Iterator begin()const
    {
        ///todo: fix it
        SuffixL2Node *node = root_->getChild(0);
        if(nullptr == node)
            return end();
        LeafNode *node2 = node->getChild(0);
        if(nullptr == node2)
            return end();
        return Iterator(node2, 0);
    }

    Iterator end()const
    {
        return Iterator();
    }

    Iterator insert(
            const KeyT &key, 
            const ValueT &val)
    {
        ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseKey(key, parsedKey))
            return end();
        size_t index = 0;

        auto insertFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                if(node->set(parsedKey[index], val))
                    ++size_;
                return SuffixTree<ContBuilderT, KeyT, ContValueT>::Iterator(node, parsedKey[index]);
            };

        return applyFunc(root_.get(), parsedKey, index, insertFunc);
    }

    Iterator find(const KeyT &key)const
    {
        ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseKey(key, parsedKey))
            return end();
        size_t index = 0;

        auto findFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                if(!node->exist(parsedKey[index]))
                    return end();
                return SuffixTree<ContBuilderT, KeyT, ContValueT>::Iterator(node, parsedKey[index]);
            };

        return applyFunc(root_.get(), parsedKey, index, findFunc);
    }

    Iterator erase(const KeyT &key)
    {
        ContBuilderT::ParsedKeyT parsedKey;
        if(!builder_.parseKey(key, parsedKey))
            return end();
        size_t index = 0;

        auto eraseFunc = [&, this](LeafNodeT *node)->ThisTypeT::Iterator
            {
                auto nextIt = SuffixTree<ContBuilderT, KeyT, ContValueT>::Iterator(node, parsedKey[index]).next();
                if(node->erase(parsedKey[index]))
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
        if(const_cast<Iterator::LeafNodeT *>(it.node())->erase(it.index()))
            --size_;
        return nextIt;
    }

    size_t size()const{return size_;}

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
                suffix_tree_impl::SuffixNode<typename ContBuilderT, 
                        static_cast<typename ContBuilderT::SuffixLevel>(ContBuilderT::leaf_Suffix - 1)> *node, 
                const typename ContBuilderT::ParsedKeyT &key, 
                size_t &index, 
                NodeFunctorT func)const
    {
        auto *childNode = node->getChild(key[index++]);
        if(nullptr == childNode)
            return end();
        return func(childNode);
    }


private:
    ///todo: implement copy operation
    SuffixTree(const SuffixTree &);
    SuffixTree &operator=(const SuffixTree &);

    BuilderT builder_;
    std::unique_ptr<RootNodeT> root_;
    size_t size_;
};


}


