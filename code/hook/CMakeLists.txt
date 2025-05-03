cmake_minimum_required(VERSION 3.10)
project(ROCF_hook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)
##
find_package(jsoncpp REQUIRED)
find_package(yaml-cpp REQUIRED)



# 添加源文件和头文件目录
add_library(ROCF_hook
    # communication with hook
    src/hook_def.cpp
    src/shared_mem_provider.cpp
    src/socket_server.cpp
    src/hook_communicator.cpp
)

target_link_libraries(ROCF_hook
    yaml-cpp
    jsoncpp
)
target_include_directories(ROCF_hook
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS ROCF_hook
    DESTINATION lib/${PROJECT_NAME}
)

add_subdirectory(test)