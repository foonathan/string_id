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
        
        /// \brief Should insert a new hash-string-pair into the internal database.
        /// \detail The string must be copied prior to stroing, it may not stay valid.<br>
        /// It should return \c false if there is already a different string stored for that hash,
        /// that is if it encounters a collision.
        virtual bool insert(hash_type hash, const char *prefix, const char* str, std::size_t length) = 0;
        
        /// \brief Should return the string stored with a given hash.
        /// \detail The return value should stay valid as long as the database exists.<br>
        /// It is guaranteed that the hash value has been inserted before.<br>
        /// If there is no way to retrieve a string it should return an error message.
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
        bool insert(hash_type, const char*, const char *, std::size_t) override
        {
            return true;
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
        
        bool insert(hash_type hash, const char *prefix, const char *str, std::size_t length) override;
        const char* lookup(hash_type hash) const noexcept override;
        
    private:        
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
        
        bool insert(hash_type hash, const char *prefix, const char *str, std::size_t length) override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return Database::insert(hash, prefix, str, length);
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
