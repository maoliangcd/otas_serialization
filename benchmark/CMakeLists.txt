cmake_minimum_required(VERSION 3.5)
project(otas_serialization)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/../3rdparty)

add_executable(test_basic test_basic.cpp)
add_executable(test_effiency_monster test_effiency_monster.cpp)
add_executable(test_effiency_person test_effiency_person.cpp)
add_executable(test_effiency_rect test_effiency_rect.cpp)

# add_subdirectory(3rdparty)