cmake_minimum_required(VERSION 3.10)
project(seed_pool)

set(CMAKE_CXX_STANDARD 20)

# 添加源文件和头文件目录
add_library(seed_pool SHARED   # 创建一个共享库
    src/seed_pool.cpp

    src/seed_pool_thread.cpp
    src/seed_pool_proc.cpp

    src/file_seed_pool_thread.cpp
    src/file_seed_pool_proc.cpp

    src/seed_pool_server.cpp
    src/seed_pool_user.cpp
)
target_include_directories(seed_pool
    PUBLIC
    /usr/include/jsoncpp
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# .so
install(TARGETS seed_pool
    DESTINATION lib
)

# test
add_subdirectory(test)