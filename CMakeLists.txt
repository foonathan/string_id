if (BIICODE)
    include(${CMAKE_HOME_DIRECTORY}/biicode.cmake)
else()
    cmake_minimum_required(VERSION 2.8)
    project(foonathan_string_id)
endif()

option(FOONATHAN_STRING_ID_DATABASE "enable or disable database" ON)
option(FOONATHAN_STRING_ID_MULTITHREADED "enable or disable a thread safe database" ON)

set(VERSION_MAJOR 2)
set(VERSION_MINOR 0)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in"
	           "${CMAKE_CURRENT_BINARY_DIR}/foonathan/string_id/config.hpp")

if (BIICODE)    
    ADD_BIICODE_TARGETS()    
    set(targets ${BII_BLOCK_TARGETS})
else()        
    set(SRC basic_database.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/foonathan/string_id/config.hpp
            database.hpp
            database.cpp
            error.cpp
            error.hpp
            generator.cpp
            generator.hpp
            hash.hpp
            string_id.cpp
            string_id.hpp)
    add_library(foonathan_string_id ${SRC})
    add_executable(foonathan_string_id_example ${SRC} example/main.cpp)
    set(targets foonathan_string_id foonathan_string_id_example)
endif()

include(cxxver.cmake)

foreach(target ${targets})
    target_compile_options(${target} PUBLIC ${CXX11_FLAG})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/foonathan/string_id)
endforeach()
