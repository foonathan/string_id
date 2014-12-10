// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "string_id.hpp"

#include "detail/database.hpp"

foonathan::string_id::string_id::string_id(const char *str, detail::basic_database &db)
: id_(detail::sid_hash(str)), db_(&db)
{
    if (!db_->insert(id_, str))
    {
        auto handler = get_collision_handler();
        auto second = db_->lookup(id_);
        handler(id_, str, second);
    }
}

const char* foonathan::string_id::string_id::string() const
{
    return db_->lookup(id_);
}
