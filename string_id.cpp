// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "string_id.hpp"

#include <cstring>

namespace sid = foonathan::string_id;

namespace
{
    void handle_collision(sid::basic_database &db, sid::hash_type hash, const char *str)
    {
        auto handler = sid::get_collision_handler();
        auto second = db.lookup(hash);
        handler(hash, str, second);
    }
}

sid::string_id::string_id(const char *str, basic_database &db)
: string_id(str, std::strlen(str), db) {}

sid::string_id::string_id(const char *str, std::size_t length, basic_database &db)
: id_(detail::sid_hash(str)), db_(&db)
{
    if (!db_->insert(id_, nullptr, str, length))
        handle_collision(*db_, id_, str);
}

sid::string_id::string_id(const string_id &prefix, const char *str)
: string_id(prefix, str, std::strlen(str)) {}

sid::string_id::string_id(const string_id &prefix, const char *str, std::size_t length)
: id_(detail::sid_hash(str, prefix.hash_code())), db_(prefix.db_)
{
    auto prefix_str = db_->lookup(prefix.hash_code());
    if (!db_->insert(id_, prefix_str, str, length + std::strlen(prefix_str)))
        handle_collision(*db_, id_, str);
}

const char* sid::string_id::string() const noexcept
{
    return db_->lookup(id_);
}

