cmake_minimum_required(VERSION 3.10)
project(header)

set(CMAKE_CXX_STANDARD 20)

# dependencies:
find_package(yaml-cpp REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(cpprestsdk REQUIRED)


# 添加源文件和头文件目录
add_library(header SHARED   # 创建一个共享库
    src/configer.cpp
    src/logger.cpp
    src/yaml_helper.cpp
    src/new_popen.cpp
)

target_include_directories(header
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


target_link_libraries(header
    yaml-cpp
    cpprest
    Boost::system
    Boost::thread
    OpenSSL::SSL
    OpenSSL::Crypto
)

# .so
install(TARGETS header
DESTINATION lib
)

# test
add_subdirectory(test)
