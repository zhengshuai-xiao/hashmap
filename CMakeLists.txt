cmake_minimum_required(VERSION 3.22)
project(CustomHashMap)

add_compile_options(-std=c++17 -Wall)
add_executable(CustomHashMap CustomHashMap.cpp CustomHashMap1.cpp unordered_map.cpp hashmap.cpp)