//
// Created by sam1203 on 6/7/19.
//
#pragma once

#include <vector>
#include <limits>
#include <memory>
#include <algorithm>
#include <functional>
#include <boost/dynamic_bitset.hpp>

namespace suffix_tree{

    namespace suffix_tree_impl{
        const size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

        template<typename MetaT, typename MetaT::SuffixLevel NodeLevelT>
        class SuffixNode;
        template<typename MetaT, typename ValueT>
        class LeafNode;

        template<typename MetaT>
        class RootNode
        {
            static const typename MetaT::SuffixLevel NODE_LEVEL = MetaT::SuffixLevel::root_Suffix;
            static const typename MetaT::SuffixLevel NEXT_NODE_LEVEL = static_cast<typename MetaT::SuffixLevel>(NODE_LEVEL + 1);
            typedef typename MetaT::template NodeTraits<NEXT_NODE_LEVEL>::NodeTypeT ChildNodeT;
            typedef ChildNodeT *ChildNodePtrT;
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

            RootNode(
                    const RootNode &nd,
                    const MetaT &metaInfo):
                    metaInfo_(metaInfo)
            {
                SubNotesT tmp;
                tmp.reserve(nd.childNodes_.size());
                std::for_each(
                        std::begin(nd.childNodes_), std::end(nd.childNodes_),
                        [&](const ChildNodeT *val){
                            if(nullptr != val){
                                /*ChildNodePtrT chld = allocator_.template allocator<NEXT_NODE_LEVEL>().
                                        make_unique(*val, allocator_, this, tmp.size(), metaInfo_);
                                tmp.emplace_back(chld.get());
                                chld.release();*/
                                tmp.emplace_back(new ChildNodeT(*val, this, tmp.size(), metaInfo_));
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
                if(nullptr == childNodes_[index]){
                    /*childNodes_[index] = allocator_.template allocator<NEXT_NODE_LEVEL>().
                            create(allocator_, this, index, metaInfo_);*/
                    childNodes_[index] = new ChildNodeT(this, index, metaInfo_);
                }
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
                if(childNodes_.size() <= index)
                    return INVALID_INDEX;

                auto childNodeCount = childNodes_.size();
                for(size_t i = index + 1; i < childNodeCount; ++i)
                {
                    if(nullptr != childNodes_[i])
                        return i;
                }
                return INVALID_INDEX;
            }

            const ChildNodeT *begin()const noexcept
            {
                auto it = std::find_if(
                        std::begin(childNodes_),
                        std::end(childNodes_),
                        [](const ChildNodeT * val){return nullptr != val;});
                if(std::end(childNodes_) == it)
                    return nullptr;
                return *it;
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

                //auto &alloc = allocator_.template allocator<NEXT_NODE_LEVEL>();
                std::for_each(std::begin(tmp), std::end(tmp),
                              [](ChildNodeT *val)
                              {
                                //alloc.destroy(val);
                                delete val;
                              });
            }

        private:
            SubNotesT childNodes_;
            const MetaT &metaInfo_;
        };

        template<typename MetaT, typename MetaT::SuffixLevel NODE_LEVEL>
        class SuffixNode
        {
            static const typename MetaT::SuffixLevel PREV_NODE_LEVEL = static_cast<typename MetaT::SuffixLevel>(NODE_LEVEL - 1);
            static const typename MetaT::SuffixLevel NEXT_NODE_LEVEL = static_cast<typename MetaT::SuffixLevel>(NODE_LEVEL + 1);
            typedef typename MetaT::template NodeTraits<PREV_NODE_LEVEL> PrevNodeTraitsT;
            typedef typename MetaT::template NodeTraits<NODE_LEVEL> NodeTraitsT;
            typedef typename MetaT::template NodeTraits<NEXT_NODE_LEVEL> NextNodeTraitsT;

            typedef typename PrevNodeTraitsT::NodeTypeT ParentNodeT;
            typedef typename NextNodeTraitsT::NodeTypeT ChildNodeT;
            typedef std::vector<ChildNodeT *> SubNotesT;

            typedef ChildNodeT *ChildNodePtrT;

        public:
            SuffixNode(
                    ParentNodeT *parentNode,
                    size_t index,
                    const MetaT &metaInfo):
                    metaInfo_(metaInfo),
                    parentNode_(parentNode), selfIndex_(index)
            {
                childNodes_.assign(metaInfo_.suffixCount(NODE_LEVEL), nullptr);
            }

            SuffixNode(
                    const SuffixNode &nd,
                    ParentNodeT *parentNode,
                    size_t index,
                    const MetaT &metaInfo):
                    metaInfo_(metaInfo),
                    parentNode_(parentNode), selfIndex_(index)
            {
                SubNotesT tmp;
                tmp.reserve(nd.childNodes_.size());
                std::for_each(
                        std::begin(nd.childNodes_), std::end(nd.childNodes_),
                        [&](const ChildNodeT *val){
                            if(nullptr != val){
                                /*ChildNodePtrT chld = allocator_.template allocator<NEXT_NODE_LEVEL>().
                                        make_unique(*val, allocator_, this, tmp.size(), metaInfo_);
                                tmp.emplace_back(chld.get());
                                chld.release();*/
                                tmp.emplace_back(new ChildNodeT(*val, this, tmp.size(), metaInfo_));
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

                if(nullptr == childNodes_[index]){
                    /*childNodes_[index] = allocator_.template allocator<NEXT_NODE_LEVEL>().
                            create(allocator_, this, index, metaInfo_);*/
                    childNodes_[index] = new ChildNodeT(this, index, metaInfo_);
                }

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
                if(childNodes_.size() <= index)
                    return INVALID_INDEX;

                auto childNodeCount = childNodes_.size();
                for(size_t i = index + 1; i < childNodeCount; ++i)
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

                //auto &alloc = allocator_.template allocator<NEXT_NODE_LEVEL>();
                std::for_each(
                        std::begin(tmp), std::end(tmp),
                        [](ChildNodeT *val)
                        {
                            //alloc.destroy(val);
                            delete val;
                        });
            }

        private:
            const MetaT &metaInfo_;
            SubNotesT childNodes_;
            ParentNodeT *parentNode_;
            size_t selfIndex_;
        };

        template<typename MetaT, typename ValueT>
        class LeafNode
        {
            static const typename MetaT::SuffixLevel PREV_NODE_LEVEL = static_cast<typename MetaT::SuffixLevel>(MetaT::SuffixLevel::leaf_Suffix - 1);
            typedef typename MetaT::template NodeTraits<PREV_NODE_LEVEL> PrevNodeTraitsT;
            typedef typename MetaT::template NodeTraits<MetaT::SuffixLevel::leaf_Suffix> NodeTraitsT;

            typedef typename PrevNodeTraitsT::NodeTypeT ParentNodeT;

            typedef std::vector<ValueT> ValuesT;

            static const bool VALUE_EXIST = true;
            static const bool VALUE_MISSED = false;
            typedef boost::dynamic_bitset<> ValueOptionalT;

        public:
            LeafNode(
                    ParentNodeT *parentNode,
                    size_t index,
                    const MetaT &metaInfo):
                    parentNode_(parentNode), selfIndex_(index)
            {
                size_t count = metaInfo.suffixCount(MetaT::SuffixLevel::leaf_Suffix);
                values_.resize(count, NodeTraitsT::defaultValue());
                optional_.resize(count, VALUE_MISSED);
            }

            LeafNode(
                    const LeafNode &nd,
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

            ~LeafNode() = default;

            LeafNode(const LeafNode &nd) = delete;
            LeafNode &operator=(const LeafNode nd) = delete;

            const ValuesT &values()const noexcept{return values_;}

            bool set(
                    size_t index,
                    const ValueT &val)
            {
                if(values_.size() <= index){
                    values_.resize(index + 1, NodeTraitsT::defaultValue());
                    optional_.resize(index + 1, VALUE_MISSED);
                    values_[index] = val;
                    optional_[index] = VALUE_EXIST;
                    return true;
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
                if((optional_.size() <= index) || (VALUE_MISSED == optional_[index]))
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
                auto idx = optional_.find_next(index);
                if(ValueOptionalT::npos == idx)
                    return INVALID_INDEX;
                return idx;
            }

            size_t begin()const noexcept
            {
                auto idx = optional_.find_first();
                if(ValueOptionalT::npos == idx)
                    return INVALID_INDEX;
                return idx;
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

}

