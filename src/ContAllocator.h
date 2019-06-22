#pragma once

//#include "SuffixTree.h"

namespace suffix_tree{
    namespace suffix_tree_impl{

        template<typename MetaT, typename AlloctT, typename MetaT::SuffixLevel level>
        class NodeAllocator{
            typedef NodeAllocator<MetaT, AlloctT, level> ThisT;
            typedef typename MetaT::template NodeTraits<AlloctT, level>::NodeTypeT NodeT;

        public:
            typedef std::unique_ptr<NodeT, std::function<void(NodeT*)>> NodePtrT;

            explicit NodeAllocator(size_t reserveSize = 100): allocNode_()
            {
            }

            template <class... Args>
            NodeT *create(Args&&... args) {
                NodeT *ptr = allocNode_.allocate(1);
                allocNode_.construct(ptr, std::forward<Args>(args)...);
                return ptr;
            }

            template <class... Args>
            NodePtrT make_unique(Args&&... args) {
                NodeT *ptr = allocNode_.allocate(1);
                allocNode_.construct(ptr, std::forward<Args>(args)...);
                return NodePtrT(ptr, [this](NodeT *node){this->destroy(node);});
            }

            void destroy(NodeT * ptr) noexcept {
                if(nullptr == ptr)
                    return;
                allocNode_.destroy(ptr);
                allocNode_.deallocate(ptr, 1);
            }

        private:
            std::allocator<NodeT> allocNode_;
        };

        template<typename MetaT, enum MetaT::SuffixLevel level>
        class TrivialNodeAllocator{
            typedef typename MetaT::template NodeTraits<level>::NodeTypeT NodeT;

        public:
            typedef std::unique_ptr<NodeT, std::function<void(NodeT*)>> NodePtrT;

            template <class... Args>
            NodeT *create(Args&&... args) {
                return new NodeT(std::forward<Args>(args)...);
            }

            template <class... Args>
            NodePtrT make_unique(Args&&... args) {
                return NodePtrT(new NodeT(std::forward<Args>(args)...));
            }

            void destroy(NodeT * ptr) noexcept{
                delete ptr;
            }
        };

        template<typename NodeT>
        class TrivialNodeAllocatorEx{
        public:
            typedef std::unique_ptr<NodeT, std::function<void(NodeT*)>> NodePtrT;

            template <class... Args>
            NodeT *create(Args&&... args) {
                return new NodeT(std::forward<Args>(args)...);
            }

            template <class... Args>
            NodePtrT make_unique(Args&&... args) {
                return NodePtrT(new NodeT(std::forward<Args>(args)...));
            }

            void destroy(NodeT * ptr) noexcept{
                delete ptr;
            }
        };

        template<typename NodeT>
        class NodeAllocatorEx{
        public:
            typedef std::unique_ptr<NodeT, std::function<void(NodeT*)>> NodePtrT;

            explicit NodeAllocatorEx(size_t reserveSize = 100): allocNode_()
            {
            }

            template <class... Args>
            NodeT *create(Args&&... args) {
                NodeT *ptr = allocNode_.allocate(1);
                allocNode_.construct(ptr, std::forward<Args>(args)...);
                return ptr;
            }

            template <class... Args>
            NodePtrT make_unique(Args&&... args) {
                NodeT *ptr = allocNode_.allocate(1);
                allocNode_.construct(ptr, std::forward<Args>(args)...);
                return NodePtrT(ptr, [this](NodeT *node){this->destroy(node);});
            }

            void destroy(NodeT * ptr) noexcept {
                if(nullptr == ptr)
                    return;
                allocNode_.destroy(ptr);
                allocNode_.deallocate(ptr, 1);
            }

        private:
            std::allocator<NodeT> allocNode_;
        };

    }

}


