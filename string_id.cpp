// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "string_id.hpp"

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

sid::string_id::string_id(string_info str, basic_database &db)
{
    basic_database::insert_status status;
    *this = string_id(str, db, status);
    if (!status)
        handle_collision(*db_, id_, str.string);
}

sid::string_id::string_id(string_info str, basic_database &db,
                          basic_database::insert_status &status)
: id_(detail::sid_hash(str.string)), db_(&db)
{
    status = db_->insert(id_, nullptr, str.string, str.length);
}

sid::string_id::string_id(const string_id &prefix, string_info str)
{
    basic_database::insert_status status;
    *this = string_id(prefix, str, status);
    if (!status)
        handle_collision(*db_, id_, str.string);
}

sid::string_id::string_id(const string_id &prefix, string_info str,
                          basic_database::insert_status &status)
: id_(detail::sid_hash(str.string, prefix.hash_code())), db_(prefix.db_)
{
    auto prefix_str = db_->lookup(prefix.hash_code());
    status = db_->insert(id_, prefix_str, str.string, str.length + std::strlen(prefix_str));
}

const char* sid::string_id::string() const noexcept
{
    return db_->lookup(id_);
}

