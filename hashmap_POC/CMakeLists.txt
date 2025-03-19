cmake_minimum_required(VERSION 3.22)
project(CustomHashMap)



add_compile_options(-g -std=c++20 -Wall -Winterference-size)

include_directories(${PROJECT_SOURCE_DIR}/POC)
#link_libraries(/usr/lib/x86_64-linux-gnu/libtcmalloc.so.4)
add_executable(CustomHashMap hashmap.cpp randomData.cc hashdict.cc IndexCache.cc CustomHashMap.cpp CustomHashMap1.cpp unordered_map.cpp )