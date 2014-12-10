// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef FOONATHAN_STRING_ID_DETAIL_DATABASE_HPP_INCLUDED
#define FOONATHAN_STRING_ID_DETAIL_DATABASE_HPP_INCLUDED

#include <mutex>
#include <string>
#include <unordered_map>

#include "hash.hpp"

namespace foonathan { namespace string_id { namespace detail
{
    class basic_database
    {
    public:
        virtual bool insert(hash_type hash, const char* str) = 0;
        virtual const char* lookup(hash_type hash) const = 0;
    };
    
    class dummy_database : public basic_database
    {
    public:        
        bool insert(hash_type, const char *) override
        {
            return true;
        }
        
        const char* lookup(hash_type) const override
        {
            return "string_id database disabled";
        }
    };
    
    class single_threaded_database : public basic_database
    {
    public:        
        bool insert(hash_type hash, const char *str) override;
        const char* lookup(hash_type hash) const override;
        
    private:
        std::unordered_map<hash_type, std::string> strings_;
    };
    
    class thread_safe_database : public basic_database
    {
    public:        
        bool insert(hash_type hash, const char *str) override;
        const char* lookup(hash_type hash) const override;
        
    private:
        single_threaded_database database_;
        mutable std::mutex mutex_;
    };
}}}

#endif // FOONATHAN_STRING_ID_DETAIL_DATABASE_HPP_INCLUDED
