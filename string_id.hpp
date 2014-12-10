// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef FOONATHAN_STRING_ID_HPP_INCLUDED
#define FOONATHAN_STRING_ID_HPP_INCLUDED

#include <functional>

#include "hash.hpp"
#include "config.hpp"
#include "database.hpp"
#include "error.hpp"

namespace foonathan { namespace string_id
{
    /// \brief The string identifier class.
    /// \detail This is a lightweight class to store strings.<br>
    /// It only stores a hash of the string allowing fast copying and comparisions.
    class string_id
    {
    public:
        //=== constructors ===//
        /// \brief Creates a new id by hashing a given string.
        /// \detail It will insert the string into the given \ref database.<br>
        /// If it encounters a collision, the \ref collision_handler will be called.
        string_id(const char *str, detail::basic_database &db);
        
        //=== accessors ===//
        /// \brief Returns the hashed value of the string.
        hash_type hash_code() const noexcept
        {
            return id_;
        }
        
        /// \brief Returns the string value itself.
        /// \detail This only works when the database is enabled,
        /// otherwise retrieving is not possible and an error message is returned.
        const char* string() const;
        
        //=== comparision ===//
        /// @{
        /// \brief Compares string ids with another or hashed values.
        /// \detail Two string ids are equal if they are from the same database and they have the same value.<br>
        /// A hashed value is equal to a string id if it is the same value.
        friend bool operator==(string_id a, string_id b) noexcept
        {
            return a.db_ == b.db_ && a.id_ == b.id_;
        }
        
        friend bool operator==(hash_type a, string_id b) noexcept
        {
            return a == b.id_;
        }
        
        friend bool operator==(string_id a, hash_type b) noexcept
        {
            return a.id_ == b;
        }
        
        friend bool operator!=(string_id a, string_id b) noexcept
        {
            return !(a == b);
        }
        
        friend bool operator!=(hash_type a, string_id b) noexcept
        {
            return !(a == b);
        }
        
        friend bool operator!=(string_id a, hash_type b) noexcept
        {
            return !(a == b);
        }
        /// @}
        
    private:
        hash_type id_;
        detail::basic_database *db_;
    };
    
    namespace literals
    {
        /// \brief A useful literal to hash a string.
        /// \detail Since this function does not check for collisions only use it to compare a \ref string_id.<br>
        /// It is also useful in places where a compile-time constant is needed.
        constexpr hash_type operator""_id(const char *str, std::size_t) noexcept
        {
            return detail::sid_hash(str);
        }
    } // literals
}} // foonathan::string_id

namespace std
{
    /// \brief \c std::hash support for \ref string_id.
    template <>
    class hash<foonathan::string_id::string_id>
    {
        using argument_type = foonathan::string_id::string_id;
        using result_type = size_t;        
 
        result_type operator()(const argument_type &arg) const noexcept
        {
            return arg.hash_code();
        }
    };
} // std

#endif // FOONATHAN_STRING_ID_HPP_INCLUDED 
