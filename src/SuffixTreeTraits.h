//
// Created by sam1203 on 4/21/19.
//

#pragma once

#include <string>
#include <cstring>
#include "ContBuilderKeys.h"
#include "SuffixTree.h"

namespace aux{

    template <size_t size> struct SuffixLevelEnum;
    template <> struct SuffixLevelEnum<0>{};
    template <> struct SuffixLevelEnum<1>{};
    template <> struct SuffixLevelEnum<2>{
        enum Levels{
            root_Suffix = 0,
            leaf_Suffix,
            total_Suffix
        };
    };
    template <> struct SuffixLevelEnum<3>{
        enum Levels{
            root_Suffix = 0,
            level1_Suffix,
            leaf_Suffix,
            total_Suffix
        };
    };
    template <> struct SuffixLevelEnum<4>{
        enum Levels{
            root_Suffix = 0,
            level1_Suffix,
            level2_Suffix,
            leaf_Suffix,
            total_Suffix
        };
    };
    template <> struct SuffixLevelEnum<5>{
        enum Levels{
            root_Suffix = 0,
            level1_Suffix,
            level2_Suffix,
            level3_Suffix,
            leaf_Suffix,
            total_Suffix
        };
    };
    template <> struct SuffixLevelEnum<6>{
        enum Levels{
            root_Suffix = 0,
            level1_Suffix,
            level2_Suffix,
            level3_Suffix,
            level4_Suffix,
            leaf_Suffix,
            total_Suffix
        };
    };
    template <> struct SuffixLevelEnum<7>{
        enum Levels{
            root_Suffix = 0,
            level1_Suffix,
            level2_Suffix,
            level3_Suffix,
            level4_Suffix,
            level5_Suffix,
            leaf_Suffix,
            total_Suffix
        };
    };

    template <size_t LevelsT, typename ContKeyT, typename ContValueT>
    class SuffixTreeTraits {
    public:
        typedef ContKeyT KeyT;
        typedef ContValueT ValueT;
        typedef SuffixTreeTraits<LevelsT, ContKeyT, ContValueT> ThisTypeT;
    public:
        static constexpr size_t NUMBER_LEVELS = LevelsT;
        typedef typename SuffixLevelEnum<LevelsT>::Levels SuffixLevel;
        typedef size_t ParsedKeyT[SuffixLevel::total_Suffix];

        template<SuffixLevel LevelIdxT, class DummyT = void>
        struct NodeTraits {
            typedef std::string KeyTypeT;
            typedef suffix_tree::suffix_tree_impl::SuffixNode<SuffixTreeTraits, LevelIdxT> NodeTypeT;
        };

        template<class DummyT>
        struct NodeTraits<ThisTypeT::SuffixLevel::root_Suffix, DummyT> {
            typedef std::string KeyTypeT;
            typedef suffix_tree::suffix_tree_impl::RootNode<SuffixTreeTraits> NodeTypeT;
        };

        template<class DummyT>
        struct NodeTraits<ThisTypeT::SuffixLevel::leaf_Suffix, DummyT> {
            typedef std::string KeyTypeT;
            typedef suffix_tree::suffix_tree_impl::LeafNode<SuffixTreeTraits, ValueT> NodeTypeT;

            static ValueT defaultValue() { return ValueT(); }
        };

        template<class DummyT>
        struct NodeTraits<ThisTypeT::SuffixLevel::total_Suffix, DummyT> {
            typedef std::string KeyTypeT;
            typedef void NodeTypeT;
        };

        explicit SuffixTreeTraits(char delimeter = '-'):
            keys_(SuffixTreeTraits::SuffixLevel::total_Suffix),
            delimeter_(delimeter)
        {}

        SuffixTreeTraits(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                char delimeter = '-'):
            keys_(lvl1, lvl2),
            delimeter_(delimeter)
        {
            static_assert(2 == LevelsT);
        }

        SuffixTreeTraits(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                char delimeter = '-'):
                keys_(lvl1, lvl2, lvl3),
                delimeter_(delimeter)
        {
            static_assert(3 == LevelsT);
        }

        SuffixTreeTraits(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                const Key2IdxT &lvl4,
                char delimeter = '-'):
                keys_(lvl1, lvl2, lvl3, lvl4),
                delimeter_(delimeter)
        {
            static_assert(4 == LevelsT);
        }

        SuffixTreeTraits(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                const Key2IdxT &lvl4,
                const Key2IdxT &lvl5,
                char delimeter = '-'):
                keys_(lvl1, lvl2, lvl3, lvl4, lvl5),
                delimeter_(delimeter)
        {
            static_assert(5 == LevelsT);
        }

        SuffixTreeTraits(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                const Key2IdxT &lvl4,
                const Key2IdxT &lvl5,
                const Key2IdxT &lvl6,
                char delimeter = '-'):
                keys_(lvl1, lvl2, lvl3, lvl4, lvl5, lvl6),
                delimeter_(delimeter)
        {
            static_assert(6 == LevelsT);
        }

        SuffixTreeTraits(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                const Key2IdxT &lvl4,
                const Key2IdxT &lvl5,
                const Key2IdxT &lvl6,
                const Key2IdxT &lvl7,
                char delimeter = '-'):
                keys_(lvl1, lvl2, lvl3, lvl4, lvl5, lvl6, lvl7),
                delimeter_(delimeter)
        {
            static_assert(7 == LevelsT);
        }

        ~SuffixTreeTraits()
        {}

        SuffixTreeTraits(const SuffixTreeTraits &cont):
                keys_(SuffixTreeTraits::SuffixLevel::total_Suffix)
        {
            delimeter_ = cont.delimeter_;
            keys_ = cont.keys_;
        }

        SuffixTreeTraits &operator=(SuffixTreeTraits cont)
        {
            delimeter_ = cont.delimeter_;
            std::swap(keys_, cont.keys_);
            return *this;
        }

        SuffixTreeTraits(SuffixTreeTraits &&) = default;
        SuffixTreeTraits &operator=(SuffixTreeTraits &&cont) = default;


        size_t levels() const noexcept
        {
            return SuffixTreeTraits::SuffixLevel::total_Suffix;
        }

        bool parseKey(
                const KeyT &key,
                SuffixTreeTraits::ParsedKeyT &res) const
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

        bool parseNewKey(
                const KeyT &key,
                SuffixTreeTraits::ParsedKeyT &res)
        {
            size_t currLevel = 0;
            size_t totalLen = key.length();
            size_t lenLeft = totalLen;
            size_t tokenLastPosition[SuffixTreeTraits::SuffixLevel::total_Suffix];
            const char *startPtr = key.c_str();
            const char *bufferPtr = startPtr;
            const char *ptr = nullptr;
            while(nullptr != (ptr = reinterpret_cast<const char *>(memchr(bufferPtr, delimeter_, lenLeft)))){
                tokenLastPosition[currLevel] = ptr - startPtr;
                ++currLevel;
                if(SuffixTreeTraits::SuffixLevel::total_Suffix <= currLevel) /// too many tokens in key
                    return false;
                lenLeft -= ptr - bufferPtr + 1;
                bufferPtr = ptr + 1;
            }

            if(SuffixTreeTraits::SuffixLevel::total_Suffix != currLevel + 1)
                return false;
            tokenLastPosition[currLevel] = totalLen;

            size_t startIdx = 0;
            for(size_t i = 0; i < SuffixTreeTraits::SuffixLevel::total_Suffix; ++i){
                size_t lastIdx = tokenLastPosition[i];
                size_t index = 0;
                getNewKeyIndex(i, key, startIdx, lastIdx, index);
                res[i] = index;
                startIdx = lastIdx + 1; ///skip delimeter
            }
            return true;
        }

        KeyT assembleKey(
                const SuffixTreeTraits::ParsedKeyT &key) const
        {
            KeyT resultKey;
            size_t currLevel = 0;
            for(auto &subKeyId: key)
            {
                const Key2IndexT &levelKeys = keys_.level(currLevel);
                auto subKeyIt = std::find_if(
                        std::begin(levelKeys), std::end(levelKeys),
                        [&](auto &v){return v.second == subKeyId;});
                if(std::end(levelKeys) != subKeyIt){
                    resultKey += delimeter_;
                    resultKey += subKeyIt->first;
                }else
                    throw std::logic_error("SuffixTreeTraits::assembleKey: unable to asseble key, subkey is unknown");
                ++currLevel;
            }
            return resultKey;
        }

        size_t suffixCount(
                SuffixLevel level) const noexcept
        {
            return keys_.suffixCount(level);
        }

    protected:
        bool getKeyIndex(
                size_t level,
                const KeyT &key,
                size_t startIdx,
                size_t endIdx,
                size_t &index) const
        {
            const Key2IndexT &levelKeys = keys_.level(level);
            KeyViewT k(key.c_str() + startIdx,  endIdx - startIdx);
            auto it = levelKeys.find(k);
            if(std::end(levelKeys) == it)
                return false;
            index = it->second;
            return true;
        }

        void getNewKeyIndex(
                size_t level,
                const KeyT &key,
                size_t startIdx,
                size_t endIdx,
                size_t &index)
        {
            const Key2IndexT &levelKeys = keys_.level(level);
            KeyViewT k(key.c_str() + startIdx,  endIdx - startIdx);
            auto it = levelKeys.find(k);
            if(std::end(levelKeys) != it){
                index = it->second;
                return;
            }
            index = keys_.addKey(level, k);
        }

    private:
        ContBuilderKeys keys_;
        char delimeter_;
    };

}


