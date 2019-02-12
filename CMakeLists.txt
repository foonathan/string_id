cmake_minimum_required(VERSION 2.8)
project(foonathan_string_id)

include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-std=c++11 cpp11_flag)
if (cpp11_flag)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    CHECK_CXX_COMPILER_FLAG(-std=c++0x cpp0x_flag)
    if (cpp0x_flag)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif(cpp0x_flag)
endif(cpp11_flag)

CHECK_CXX_SOURCE_COMPILES("constexpr auto foo = 1; int main(){}" comp_constexpr)
CHECK_CXX_SOURCE_COMPILES("void foo() noexcept {} int main(){}" comp_noexcept)
CHECK_CXX_SOURCE_COMPILES("#include <cstddef>
                           int operator\"\"_foo(const char *, std::size_t){return 0;} int main(){}" comp_literal)
CHECK_CXX_SOURCE_COMPILES("struct base {virtual void foo() {}};
                           struct derived : base {void foo() override {}};
                           int main(){}" comp_override)

option(FOONATHAN_STRING_ID_DATABASE "enable or disable database" ON)
option(FOONATHAN_STRING_ID_MULTITHREADED "enable or disable a thread safe database" ON)
option(FOONATHAN_IMPL_HAS_CONSTEXPR "whether or not constexpr is supported" ${comp_constexpr})
option(FOONATHAN_IMPL_HAS_NOEXCEPT "whether or not noexcept is supported" ${comp_noexcept})
option(FOONATHAN_IMPL_HAS_LITERAL "whether or not literal operator overloading is supported" ${comp_literal})
option(FOONATHAN_IMPL_HAS_OVERRIDE "whether or not override is supported" ${comp_override})
if (NOT CMAKE_COMPILER_IS_GNUCXX)
    set(atomic_handler ON CACHE INTERNAL "")
elseif(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.7 OR
        CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7)
    set(atomic_handler ON CACHE INTERNAL "")
else()
    # not supported on GCC <= 4.6
    set(atomic_handler OFF CACHE INTERNAL "")
endif(NOT CMAKE_COMPILER_IS_GNUCXX)
option(FOONATHAN_STRING_ID_ATOMIC_HANDLER "whether or not handler functions are atomic" ${atomic_handler})

set(version_major 2 CACHE INTERNAL "")
set(version_minor 0 CACHE INTERNAL "")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in"
	           "${CMAKE_CURRENT_BINARY_DIR}/config_impl.hpp")

set(src basic_database.hpp
        config.hpp
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
add_executable(foonathan_string_id_example example/main.cpp)
target_link_libraries(foonathan_string_id_example PUBLIC foonathan_string_id)

set(targets foonathan_string_id foonathan_string_id_example CACHE INTERNAL "")

set_target_properties(${targets} PROPERTIES CXX_STANDARD 11)

foreach(target ${targets})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
