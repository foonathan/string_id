// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "string_id.hpp"

#include <cstring>

namespace sid = foonathan::string_id;

sid::string_id::string_id(const char *str, basic_database &db)
: string_id(str, std::strlen(str), db) {}

sid::string_id::string_id(const char *str, std::size_t length, basic_database &db)
: id_(detail::sid_hash(str)), db_(&db)
{
    if (!db_->insert(id_, str, length))
    {
        auto handler = get_collision_handler();
        auto second = db_->lookup(id_);
        handler(id_, str, second);
    }
}

const char* sid::string_id::string() const noexcept
{
    return db_->lookup(id_);
}
