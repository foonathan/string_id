// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef FOONATHAN_STRING_ID_DATABASE_HPP_INCLUDED
#define FOONATHAN_STRING_ID_DATABASE_HPP_INCLUDED

#include <memory>
#include <mutex>
#include <unordered_map>

#include "config.hpp"
#include "hash.hpp"

namespace foonathan { namespace string_id
{       
    /// \brief The interface for all databases.
    /// \detail You can derive own databases from it.
    class basic_database
    {
    public:        
        /// @{
        /// \brief Databases are not copy- or moveable.
        /// \detail You must not write a swap function either!<br>
        /// This has implementation reasons.
        basic_database(const basic_database &) = delete;
        basic_database(basic_database &&) = delete;
        /// @}
        
        /// \brief The status of an insert operation.
        enum insert_status
        {
            /// \brief Two different strings collide on the same value.
            collision,
            /// \brief A new string was inserted.
            new_string,
            /// \brief The string already existed inside the database.
            old_string
        };
        
        /// \brief Should insert a new hash-string-pair with prefix (optional) into the internal database.
        /// \detail The string must be copied prior to storing, it may not stay valid.
        /// \arg hash is the hash of the string.
        /// \arg str is the string which does not need to be null-terminated.
        /// \arg length is the length of the string.
        /// \return The \ref insert_status.
        virtual insert_status insert(hash_type hash, const char* str, std::size_t length) = 0;
        
        /// \brief Inserts a hash-string-pair with given prefix into the internal database.
        /// \detail The default implementation performs a lookup of the prefix string and appends it,
        /// then it calls \ref insert.<br>
        /// Override it if you can do it more efficiently.
        /// \arg hash is the hash of the string plus prefix.
        /// \arg prefix is the hash of the prefix-string.
        /// \arg str is the suffix which does not need to be null-terminated.
        /// \arg length is the length of the suffix.
        /// \return The \ref insert_status.
        virtual insert_status insert_prefix(hash_type hash, hash_type prefix,
                                            const char *str, std::size_t length);
        
        /// \brief Should return the string stored with a given hash.
        /// \detail It is guaranteed that the hash value has been inserted before.
        /// \return A null-terminated string belonging to the hash code or
        /// an error message if the database does not store anything.<br>
        /// The return value must stay valid as long as the database exists.
        virtual const char* lookup(hash_type hash) const noexcept = 0;
        
    protected:
        basic_database() noexcept = default;
        ~basic_database() noexcept = default;
    };
    
    /// \brief A database that doesn't store the string-values.
    /// \detail It does not detect collisions or allows retrieving,
    /// \c lookup() returns "string_id database disabled".
    class dummy_database : public basic_database
    {
    public:        
        insert_status insert(hash_type, const char *, std::size_t) override
        {
            return new_string;
        }
        
        insert_status insert_prefix(hash_type, hash_type, const char *, std::size_t) override
        {
            return new_string;
        }
        
        const char* lookup(hash_type) const noexcept override
        {
            return "string_id database disabled";
        }
    };
    
    /// \brief A database that uses a highly optimized hash table.
    class map_database : public basic_database
    {
    public:        
        /// \brief Creates a new database with given number of buckets and maximum load factor.
        map_database(std::size_t size = 1024, double max_load_factor = 1.0);
        ~map_database() noexcept;
        
        insert_status insert(hash_type hash, const char *str, std::size_t length) override;
        insert_status insert_prefix(hash_type hash, hash_type prefix,
                                    const char *str, std::size_t length) override;
        const char* lookup(hash_type hash) const noexcept override;
        
    private:        
        void rehash();
        
        class node_list;
        std::unique_ptr<node_list[]> buckets_;
        std::size_t no_items_, no_buckets_;
        double max_load_factor_;
        std::size_t next_resize_;
    };
    
    /// \brief A thread-safe database adapter.
    /// \detail It derives from any database type and synchronizes access via \c std::mutex.
    template <class Database>
    class thread_safe_database : public Database
    {
    public:
        /// \brief The base database.
        using base_database = Database;
        
        using Database::Database;
        
        auto insert(hash_type hash, const char *str, std::size_t length) 
        -> typename Database::insert_status override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return Database::insert(hash, str, length);
        }
        
        auto insert_prefix(hash_type hash, hash_type prefix, const char *str, std::size_t length) 
        -> typename Database::insert_status override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return Database::insert_prefix(hash, prefix, str, length);
        }
        
        const char* lookup(hash_type hash) const noexcept override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return Database::lookup(hash);
        }
        
    private:
        mutable std::mutex mutex_;
    };
    
    /// \brief The default database where the strings are stored.
    /// \detail Its exact type is one of the previous listed databases.
    /// You can control its selection via the macros listed in config.hpp.in.
#if FOONATHAN_STRING_ID_DATABASE && FOONATHAN_STRING_ID_MULTITHREADED
    using default_database = thread_safe_database<map_database>;
#elif FOONATHAN_STRING_ID_DATABASE
    using default_database = map_database;
#else
    using default_database = dummy_database;
#endif
}} // namespace foonathan::string_id

#endif // FOONATHAN_STRING_ID_DATABASE_HPP_INCLUDED
