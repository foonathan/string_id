// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "error.hpp"

#include <atomic>
#include <sstream>

namespace sid = foonathan::string_id;

namespace
{    
    [[noreturn]] void default_collision_handler(sid::hash_type hash, const char *a, const char *b)
    {
        throw sid::collision_error(hash, a, b);
    }
    
    std::atomic<sid::collision_handler> collision_h(default_collision_handler);
}

sid::collision_handler sid::set_collision_handler(collision_handler h)
{
    return collision_h.exchange(h);
}

sid::collision_handler sid::get_collision_handler()
{
    return collision_h;
}

const char* sid::collision_error::what() const noexcept try
{
    std::ostringstream str;
    str << "foonathan::string_id::collision_error: strings "
        << '"' << a_ << '"' << " and " << '"' << b_ << '"'
        << " are both producing the value " << hash_ << '.';
    return str.str().c_str();
}
catch (...)
{
    return "foonathan::string_id::collision_error: two different strings are producing the same value";
}

namespace
{
    constexpr auto no_tries_generation = 8u;
    
    bool default_generation_error_handler(std::size_t no, const char *generator_name,
                                          sid::hash_type, const char *)
    {
        if (no >= no_tries_generation)
            throw sid::generation_error(generator_name);
        return true;
    }
    
    std::atomic<sid::generation_error_handler> generation_error_h(default_generation_error_handler);
}

sid::generation_error_handler sid::set_generation_error_handler(generation_error_handler h)
{
    return generation_error_h.exchange(h);
}

sid::generation_error_handler sid::get_generation_error_handler()
{
    return generation_error_h;
}

const char* sid::generation_error::what() const noexcept try
{
    return ("foonathan::string_id::generation_error: Generator \"" + name_ +
            "\" was unable to generate new string id.").c_str();
}
catch (...)
{
    return "foonathan::string_id::generation_error: unable to generate new string id.";
}
