cmake_minimum_required(VERSION 3.10)
project(data_provider)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)  


# 添加源文件和头文件目录
add_library(data_provider SHARED
    src/data_provider_thread.cpp
)


target_include_directories(data_provider
    PUBLIC
    /usr/include/jsoncpp
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖的库
target_link_libraries(data_provider
  PUBLIC
  mutator
  seed_pool
  header
)

# .so
install(TARGETS data_provider
  DESTINATION lib
)