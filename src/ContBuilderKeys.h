//
// Created by sam1203 on 4/11/19.
//

#pragma once

#include "StringArena.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace aux {

    typedef std::string KeyT;
    typedef std::string_view KeyViewT;
    typedef std::vector<KeyT> Key2IdxT;
    typedef std::unordered_map<KeyViewT, size_t> Key2IndexT;


    class ContBuilderKeys{
    public:
        explicit ContBuilderKeys(size_t levelCount);
        ContBuilderKeys(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2);
        ContBuilderKeys(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3);
        ContBuilderKeys(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                const Key2IdxT &lvl4);
        ContBuilderKeys(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                const Key2IdxT &lvl4,
                const Key2IdxT &lvl5);
        ContBuilderKeys(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                const Key2IdxT &lvl4,
                const Key2IdxT &lvl5,
                const Key2IdxT &lvl6);
        ContBuilderKeys(
                const Key2IdxT &lvl1,
                const Key2IdxT &lvl2,
                const Key2IdxT &lvl3,
                const Key2IdxT &lvl4,
                const Key2IdxT &lvl5,
                const Key2IdxT &lvl6,
                const Key2IdxT &lvl7);

        ContBuilderKeys(const ContBuilderKeys &);
        ContBuilderKeys &operator=(const ContBuilderKeys &cont);

        ContBuilderKeys(ContBuilderKeys &&) = default;
        ContBuilderKeys &operator=(ContBuilderKeys &&cont) = default;

        size_t suffixCount(
                size_t level)const noexcept;

        const Key2IndexT &level(size_t level)const noexcept;

        size_t addKey(size_t level, const KeyViewT &val);
    private:
        StringArena stringAllocator_;

        typedef std::vector<Key2IndexT> MetaDataPerLevelsT;
        MetaDataPerLevelsT meta_;
    };

}


