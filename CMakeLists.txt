option(FOONATHAN_STRING_ID_DATABASE "enable or disable database" ON)
option(FOONATHAN_STRING_ID_MULTITHREADED "enable or disable a thread safe database" ON)

if (BIICODE)
    include(${CMAKE_HOME_DIRECTORY}/biicode.cmake)
    INIT_BIICODE_BLOCK()
    
    BII_CONFIGURE_FILE(config.hpp.in config.hpp)
    
    ADD_BIICODE_TARGETS()    
    set(targets ${BII_BLOCK_TARGETS})
else()
    cmake_minimum_required(VERSION 2.8)
    project(foonathan_string_id)
    
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in"
	               "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")
    
    set(SRC ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
            database.hpp
            database.cpp
            error.cpp
            error.hpp
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
    target_include_directories(${target} PUBLIC ${CMAKE_BINARY_DIR})
endforeach()
