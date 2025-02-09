set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

#  test: data-provider--------#
add_executable(data_provider_test
    data_provider_test.cpp
)
target_link_libraries(data_provider_test
    PRIVATE
    data_provider
)