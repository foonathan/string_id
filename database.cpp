#include "database.hpp"

namespace sid = foonathan::string_id;

bool sid::map_database::insert(hash_type hash, const char *str)
{
    auto iter = strings_.find(hash);
    if (iter != strings_.end())
        return iter->second == str;
    strings_.emplace(hash, str);
    return true;
}

const char* sid::map_database::lookup(hash_type hash) const noexcept
{
    return strings_.at(hash).c_str();
}
