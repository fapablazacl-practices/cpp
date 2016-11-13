cmake_minimum_required(VERSION 3.1)

project (cpp)

set (CMAKE_CXX_STANDARD 11)

#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_STATIC_RUNTIME OFF)

#find_package(Boost COMPONENTS phoenix REQUIRED)
#find_package(Boost REQUIRED)
#include_directories (${Boost_INCLUDE_DIR})

add_subdirectory(lazy)
