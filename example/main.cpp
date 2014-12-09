#include <iostream>

// main header, only need to include it
#include "../string_id.hpp"

// namespace alias
namespace sid = foonathan::string_id;

int main() try
{
    // this allows using the literal
    using namespace sid::literals;
    
    // create database to store the strings in
    // must stay valid as long as each string_id using it
    sid::database database;
    
    // create an id
    sid::string_id id("Test0815", database);
    std::cout << "Hash code " << id.hash_code() << " belongs to string \"" << id.string() << "\"\n";
    // Output (Database enabled): Hash code 16741300784925887095 belongs to string "Test0815"
    // Output (Database disabled): Hash code 16741300784925887095 belongs to string "string_id database disabled"
    
    sid::string_id a("Hello", database), b("World", database);
    
    // compare two ids
    std::cout << std::boolalpha << (a == b) << '\n';
    // Output: false
    
    // compare id with constant
    std::cout << (a == "Hello"_id) << '\n';
    // Output: true
    
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
}
catch (sid::collision_error &ex)
{
    // can only be thrown when database is enabled and is not very likely to occur
    std::cerr << "Collision! " << ex.what() << '\n';
}
