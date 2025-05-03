cmake_minimum_required(VERSION 3.10)

project(mutator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "clang++")

# 查找依赖包
find_package(yaml-cpp REQUIRED)

add_library(mutator SHARED 
    src/mutator.cc
    src/knobs.cc
    src/yaml_mutator.cpp
)

target_include_directories(mutator
    PUBLIC
    /usr/include/jsoncpp
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(mutator 
    PUBLIC
    yaml-cpp
)

# .so
install(TARGETS mutator
    DESTINATION lib
)

add_subdirectory(test)