cmake_minimum_required(VERSION 4.0)
project(otas_serialization)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/../include)

add_executable(test_1 test_1.cpp)
add_executable(test_2 test_2.cpp)
add_executable(test_3 test_3.cpp)
add_executable(test_effiency_vec test_effiency_vec.cpp)
add_executable(test_effiency_map test_effiency_map.cpp)
add_executable(test_effiency_array test_effiency_array.cpp)