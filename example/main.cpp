// Copyright (C) 2014-2015 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <ctime> // std::time
#include <iostream>
#include <random> // std::mt19937
#include <vector>

#include "../database.hpp" // for the databases
#include "../error.hpp" // for error handling
#include "../generator.hpp" // for the generator classes
#include "../string_id.hpp" // for the string_id

// namespace alias
namespace sid = foonathan::string_id;

int main() try
{
    // this allows using the literal
    using namespace sid::literals;
    
    // create database to store the strings in
    // it must stay valid as long as each string_id using it
    sid::default_database database;
    
    //=== string_id usage ===//
    // create an id
    sid::string_id sid("Test0815", database);
    std::cout << "Hash code " << sid.hash_code() << " belongs to string \"" << sid.string() << "\"\n";
    // Output (Database supports retrieving): Hash code 16741300784925887095 belongs to string "Test0815"
    // Output (Database doesn't): Hash code 16741300784925887095 belongs to string "string_id database disabled"
    
    sid::string_id a("Hello", database), b("World", database);
    
    // compare two ids
    std::cout << std::boolalpha << (a == b) << '\n';
    // Output: false
    
    // compare id with constant
#if FOONATHAN_STRING_ID_HAS_LITERAL
    std::cout << (a == "Hello"_id) << '\n';
#else
    std::cout << (a == id("Hello")) << '\n';
#endif
    // Output: true
    
#if FOONATHAN_STRING_ID_HAS_LITERAL
    // literal is compile-time
    switch (b.hash_code())
    {
    case "Hello"_id:
        std::cout << "Hello\n";
        break;
    case "world"_id: // case-sensitive
        std::cout << "world\n";
        break;
    case "World"_id:
        std::cout << "World\n";
        break;
    }
#endif
    
    //=== generation ===//
    // the prefix for all generated ids
    sid::string_id prefix("entity-", database);
    try
    {
        // a generator type appending 8 random characters to the prefix
        // it uses the std::mt19937 generator for the actual generation
        typedef sid::random_generator<std::mt19937, 8> generator_t;
        // create a generator, seed the random number generator with the current time
        generator_t generator(prefix, std::mt19937(std::int_fast32_t(std::time(nullptr))));
        
        std::vector<sid::string_id> ids;
        for (auto i = 0; i != 10; ++i)
            // generate new identifier
            // it is guaranteed unique and will be stored in the database of the prefix
            ids.push_back(generator());
        
        // print the name of all the ids
        for (auto &id : ids)
            std::cout << id.string() << '\n';
        // possible generated name: entity-jXRnZAVG
    }
    catch (sid::generation_error &ex)
    {
        // the generator was unable to generate a new unique identifier (very unlikely)
        std::cerr << "[ERROR] " << ex.what() << '\n';
    }
    
    try
    {
        // a generator appending an increasing number to the prefix
        typedef sid::counter_generator generator_t;
        
        // create a generator starting with 0, each number will be 4 digits long
        generator_t generator(prefix, 0, 4);
        
        std::vector<sid::string_id> ids;
        for (auto i = 0; i != 10; ++i)
            // generate new identifier
            // it is guaranteed unique and will be stored in the database of the prefix
            ids.push_back(generator());
        
        // print the name of all the ids
        for (auto &id : ids)
            std::cout << id.string() << '\n';
        // possible generated name: entity-0006
    }
    catch (sid::generation_error &ex)
    {
        // the generator was unable to generate a new unique identifier (very unlikely)
        std::cerr << "[ERROR] " << ex.what() << '\n';
    }
}
catch (sid::collision_error &ex)
{
    // two different strings are resulting in the same hash value (very unlikely)
    std::cerr << "[ERROR] " << ex.what() << '\n';
}
