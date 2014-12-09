// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef FOONATHAN_STRING_ID_ERROR_HPP_INCLUDED
#define FOONATHAN_STRING_ID_ERROR_HPP_INCLUDED

#include <stdexcept>

#include "detail/hash.hpp"

namespace foonathan { namespace string_id
{
    /// \brief The type of the collision handler.
    /// \detail It will be called when a string hashing results in a collision giving it the two strings collided.
    /// The default handler throws an exception of type \ref collision_error.
    using collision_handler = void(*)(detail::hash_type hash, const char *a, const char *b);
    
    /// \brief Exchanges the \ref collision_handler.
    /// \detail This function is thread safe.
    collision_handler set_collision_handler(collision_handler h);
    
    /// \brief Returns the current collision handler.
    collision_handler get_collision_handler();
    
    /// \brief The exception class thrown by the default \ref collision_handler.
    class collision_error : public std::logic_error
    {
    public:
        //=== constructor/destructor ===//
        /// \brief Creates a new exception, same parameter as \ref collision_handler.
        collision_error(detail::hash_type hash, const char *a, const char *b);
        
        ~collision_error() noexcept override = default;
        
        //=== accessors ===//
        /// @{
        /// \brief Returns one of the two strings that colllided.
        const char* first_string() const noexcept
        {
            return a_.c_str();
        }
        
        const char* second_string() const noexcept
        {
            return b_.c_str();
        }
        /// @}
        
        /// \brief Returns the hash code of the collided strings.
        detail::hash_type hash_code() const noexcept
        {
            return hash_;
        }
        
    private:
        std::string a_, b_;
        detail::hash_type hash_;
    };
}} // namespace foonathan::string_id

#endif // FOONATHAN_STRING_ID_ERROR_HPP_INCLUDED
