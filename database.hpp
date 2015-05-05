// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef FOONATHAN_STRING_ID_DATABASE_HPP_INCLUDED
#define FOONATHAN_STRING_ID_DATABASE_HPP_INCLUDED

#include <memory>
#include <mutex>

#include "basic_database.hpp"
#include "config.hpp"

namespace foonathan { namespace string_id
{    
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
        
        const char* lookup(hash_type) const FOONATHAN_NOEXCEPT override
        {
            return "string_id database disabled";
        }
    };
    
    /// \brief A database that uses a highly optimized hash table.
    class map_database : public basic_database
    {
    public:        
        /// \brief Creates a new database with given number of buckets and maximum load factor.
        explicit map_database(std::size_t size = 1024, double max_load_factor = 1.0);
        ~map_database() FOONATHAN_NOEXCEPT;
        
        insert_status insert(hash_type hash, const char *str, std::size_t length) override;
        insert_status insert_prefix(hash_type hash, hash_type prefix,
                                    const char *str, std::size_t length) override;
        const char* lookup(hash_type hash) const FOONATHAN_NOEXCEPT override;
        
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
        
        // workaround of lacking inheriting constructors
		template <typename ... Args>
        explicit thread_safe_database(Args&&... args)
		: base_database(std::forward<Args>(args)...) {}
        
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
        
        const char* lookup(hash_type hash) const FOONATHAN_NOEXCEPT override
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
