cmake_minimum_required(VERSION 3.10)
project(usr_input)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)

find_package(yaml-cpp REQUIRED)

# 添加源文件和头文件目录
add_library(usr_input SHARED
    src/ROCF_usr_input.cpp
    src/usr_input.cpp
    src/queue_usr_input.cpp
)
target_include_directories(usr_input
    PUBLIC
    /usr/include/jsoncpp
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


# 链接依赖的库
target_link_libraries(usr_input
  PUBLIC
  yaml-cpp
  config
  multi_process_manager
)

# .so
install(TARGETS usr_input
  DESTINATION lib
)


#add_subdirectory(test)