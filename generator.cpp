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
    sid::string_info to_string(sid::counter_generator::state s, char *begin, char *end,
                               std::size_t length)
    {
        *--end = 0;
        auto cur = end;
        std::size_t i = 0;
        
        do
        {
            *--cur = '0' + (s % 10);
            s /= 10;
            ++i;
        } while (s != 0u);
        
        if (i < length)
            for (; cur - 1 != begin && i < length; ++i)
                *--cur = '0';
        else if (length && i > length)
            cur += i - length;
            
        return sid::string_info(cur, end - cur);
    }
}

sid::string_id sid::counter_generator::operator()()
{
    // 4 times sizeof(state) is enough for the integer representation
    static constexpr auto max_size = 4 * sizeof(state);
    char string[max_size];
    return detail::try_generate("foonathan::string_id::counter_generator",
                                [&]()
                                {
                                    return to_string(counter_++, string, string + max_size, length_);
                                }, prefix_);
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
