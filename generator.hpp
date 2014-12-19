// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef FOONATHAN_STRING_ID_GENERATOR_HPP_INCLUDED
#define FOONATHAN_STRING_ID_GENERATOR_HPP_INCLUDED

#include <atomic>
#include <random>

#include "string_id.hpp"

namespace foonathan { namespace string_id
{
    template <class Generator>
    class generator
    {
    public:
        using state = Generator;
        
        explicit generator(const string_id &prefix,
                           state s = state())
        : prefix_(prefix), state_(s) {}
        
        string_id operator()()
        {
            return {prefix_, state_()};
        }
        
        void discard(unsigned long long n)
        {
            state_.discard(n);
        }
        
    private:
        string_id prefix_;
        state state_;
    };
    
    class counter_generator
    {
    public:
        using state = unsigned long long;
        
        explicit counter_generator(const string_id &prefix,
                                   state counter = {}, std::size_t length = 0)
        : prefix_(prefix), counter_(counter), length_(length) {}
        
        string_id operator()();
                
        void discard(unsigned long long n) noexcept;
        
    private:
        string_id prefix_;
        std::atomic<state> counter_;
        std::size_t length_;
    };
    
    struct character_table
    {
        const char* characters;
        std::size_t no_characters;
        
        constexpr character_table(const char* chars, std::size_t no) noexcept
        : characters(chars), no_characters(no) {}
        
        static character_table alnum() noexcept;
        static character_table alpha() noexcept;
    };
    
    template <class RandomNumberGenerator, std::size_t Length>
    class random_generator
    {        
    public:        
        using state = RandomNumberGenerator;
        
        static constexpr std::size_t length() noexcept
        {
            return Length;
        }
        
        explicit random_generator(const string_id &prefix,
                                  state s = state(),
                                  character_table table = character_table::alnum())
        : prefix_(prefix), state_(std::move(s)),
          table_(table) {}
        
        string_id operator()()
        {
            std::uniform_int_distribution<std::size_t>
                dist(0, table_.no_characters - 1);
            char random[Length + 1];
            random[Length] = 0;
            for (std::size_t i = 0u; i != Length; ++i)
                random[i] = table_.characters[dist(state_)];
            return {prefix_, random, Length};
        }
        
        void discard(unsigned long long n)
        {
            state_.discard(n);
        }
        
    private:
        string_id prefix_;
        state state_;
        character_table table_;
    };
}} // namespace foonathan::string_id

#endif // FOONATHAN_STRING_ID_GENERATOR_HPP_INCLUDED
