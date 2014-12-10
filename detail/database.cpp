// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "database.hpp"

namespace sid = foonathan::string_id;

bool sid::detail::single_threaded_database::insert(hash_type hash, const char *str)
{
    auto iter = strings_.find(hash);
    if (iter != strings_.end())
        return iter->second == str;
    strings_.emplace(hash, str);
    return true;
}

const char* sid::detail::single_threaded_database::lookup(hash_type hash) const
{
    return strings_.at(hash).c_str();
}

bool sid::detail::thread_safe_database::insert(hash_type hash, const char* str)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return database_.insert(hash, str);
}

const char* sid::detail::thread_safe_database::lookup(hash_type hash) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return database_.lookup(hash);
}
