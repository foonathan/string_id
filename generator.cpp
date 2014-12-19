// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "generator.hpp"

#include <cassert>
#include <cstring>
#include <utility>

namespace sid = foonathan::string_id;

namespace
{
    std::pair<char*, std::size_t> to_string(char *begin, char *end,
                                            sid::counter_generator::state s) noexcept
    {
        *--end = 0;
        auto cur = end;
        while (--cur != begin)
        {
            *cur = (s % 10) + '0';
            s /= 10;
            if (s == 0u)
                return {cur, end - cur};
        }
        assert(false && "not sufficient array size");
        return {};
    }
}

sid::string_id sid::counter_generator::operator()()
{
    // 4 times sizeof(s) is enough for the integer representation
    static constexpr auto max_size = 4 * sizeof(state) + 1;
    char string[max_size];
    auto number = to_string(string, string + max_size, counter_++);
    if (length_ != 0)
        if (number.second < length_)
        {
            auto to_fill = length_ - number.second;
            number.first = number.first - to_fill;
            number.second = to_fill;
            std::memset(number.first, '0', to_fill);
        }
        else if (number.second > length_)
        {
            number.first = string + max_size - length_ - 1;
            number.second = length_;
        }
    return {prefix_, number.first, number.second};
}

void sid::counter_generator::discard(unsigned long long n) noexcept
{
    counter_ += n;
}

namespace
{
    static constexpr char table[]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxzy0123456789";
}

sid::character_table sid::character_table::alnum() noexcept
{
    return {table, sizeof(table) - 1};
}

sid::character_table sid::character_table::alpha() noexcept
{
    return {table, sizeof(table) - 1 - 10};
}
