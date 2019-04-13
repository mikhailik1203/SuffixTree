//
// Created by sam1203 on 4/11/19.
//

#pragma once

#include <vector>
#include <string>
#include <memory>

namespace aux{

    class StringArena {
    public:
        StringArena(
                unsigned alignBytes,
                size_t buffer_size,
                double factor,
                size_t limit);
        ~StringArena();

        StringArena(const StringArena &) = delete;
        StringArena &operator=(const StringArena &cont) = delete;

        StringArena(StringArena &&) = default;
        StringArena &operator=(StringArena &&cont) = default;

        std::string_view allocate(const std::string_view &val);

    private:
        void allocate(size_t size);
        void clear();

    private:
        typedef std::vector<std::unique_ptr<const char[]>> BlocksT;
        BlocksT allocated_;
        char *buffer_;
        int64_t sizeLeft_;

        size_t limitSize_;
        size_t bufferSize_;
        double factor_;
        unsigned align_;
    };

}


