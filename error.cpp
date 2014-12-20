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

namespace
{
    std::string format_collision(sid::hash_type hash, const char *a, const char *b)
    {
        std::ostringstream str;
        str << "foonathan::string_id::collision_error: strings "
            << '"' << a << '"' << " and " << '"' << b << '"'
            << " are both producing the value " << hash << '.';
        return str.str();
    }
}

sid::collision_error::collision_error(hash_type hash, const char *a, const char *b)
: error(format_collision(hash, a, b)), a_(a), b_(b), hash_(hash)
{}

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

namespace
{
    std::string format_generation(std::string name)
    {
        return "foonathan::string_id::generation_error: Generator \"" + name +
               "\" was unable to generate new string id.";
    }
}

sid::generation_error::generation_error(const char *name)
: error(format_generation(name)), name_(name)
{}
