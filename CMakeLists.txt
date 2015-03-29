if (BIICODE)
    include(${CMAKE_HOME_DIRECTORY}/biicode.cmake)
    include(biicode/cmake/tools)
else()
    cmake_minimum_required(VERSION 3.0)
    project(foonathan_string_id)
endif()

option(FOONATHAN_STRING_ID_DATABASE "enable or disable database" ON)
option(FOONATHAN_STRING_ID_MULTITHREADED "enable or disable a thread safe database" ON)

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
