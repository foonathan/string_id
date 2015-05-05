if (BIICODE)
    include(${CMAKE_HOME_DIRECTORY}/biicode.cmake)
    include(biicode/cmake/tools)
else()
    cmake_minimum_required(VERSION 3.0)
    project(foonathan_string_id)
endif()

include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_FLAGS "-std=c++11" CACHE INTERNAL "")
CHECK_CXX_SOURCE_COMPILES("constexpr auto foo = 1; int main(){}" comp_constexpr)
CHECK_CXX_SOURCE_COMPILES("void foo() noexcept {} int main(){}" comp_noexcept)
CHECK_CXX_SOURCE_COMPILES("#include <cstddef>
                           int operator\"\"_foo(const char *, std::size_t){return 0;} int main(){}" comp_literal)

option(FOONATHAN_STRING_ID_DATABASE "enable or disable database" ON)
option(FOONATHAN_STRING_ID_MULTITHREADED "enable or disable a thread safe database" ON)
option(FOONATHAN_IMPL_HAS_CONSTEXPR "whether or not constexpr is supported" ${comp_constexpr})
option(FOONATHAN_IMPL_HAS_NOEXCEPT "whether or not noexcept is supported" ${comp_noexcept})
option(FOONATHAN_IMPL_HAS_LITERAL "whether or not literal operator overloading is supported" ${comp_literal})

set(version_major 2 CACHE INTERNAL "")
set(version_minor 0 CACHE INTERNAL "")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in"
	           "${CMAKE_CURRENT_BINARY_DIR}/foonathan/string_id/config.hpp")

if (BIICODE)    
    ADD_BIICODE_TARGETS()    
    set(targets ${BII_BLOCK_TARGETS} CACHE INTERNAL "")
else()        
    set(src basic_database.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/foonathan/string_id/config.hpp
            database.hpp
            database.cpp
            error.cpp
            error.hpp
            generator.cpp
            generator.hpp
            hash.hpp
            string_id.cpp
            string_id.hpp
        CACHE INTERNAL "")

    add_library(foonathan_string_id ${src})
    add_executable(foonathan_string_id_example ${src} example/main.cpp)

    set(targets foonathan_string_id foonathan_string_id_example CACHE INTERNAL "")
    
    set_target_properties(${targets} PROPERTIES CXX_STANDARD 11)
    set_target_properties(${targets} PROPERTIES CXX_STANDARD_REQUIRED ON)
endif()

foreach(target ${targets})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/foonathan/string_id)
    if (BIICODE)
        ACTIVATE_CPP11(${target})
    endif()
endforeach()
