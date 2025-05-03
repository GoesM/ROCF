# test
#  test:seed_pool --------#
add_executable(test_seed_pool
    test_seed_pool.cpp
)
target_link_libraries(test_seed_pool
    PRIVATE
    seed_pool
)

#  test:seed_pool_server--------#
add_executable(test_seed_pool_proc
    test_seed_pool_proc.cpp
)
target_link_libraries(test_seed_pool_proc
    PRIVATE
    seed_pool
)
#  test:file_seed_pool_manager serviceAPI--------#
add_executable(test_file_seed_pool_proc
    test_file_seed_pool_proc.cpp
)
target_link_libraries(test_file_seed_pool_proc
    PRIVATE
    seed_pool
)
#  test:seed_pool_manager UserAPI--------#
add_executable(test_seed_pool_user
    test_seed_pool_user.cpp
)
target_link_libraries(test_seed_pool_user
    PRIVATE
    seed_pool
)

# all 
add_executable(test_all
    test_all.cpp
)
target_link_libraries(test_all
    PRIVATE
    seed_pool
)




# test ---
install(TARGETS  # 安装节点 install
    test_seed_pool   
    test_seed_pool_proc
    test_file_seed_pool_proc
    test_seed_pool_user
    
    test_all

    DESTINATION lib/${PROJECT_NAME}
)

