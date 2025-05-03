
# test---- 
add_executable(test_queue_usr_input
    test/test_queue_usr_input.cpp
)
target_link_libraries(test_queue_usr_input
    PRIVATE
    usr_input
)
install(TARGETS 
    test_queue_usr_input   # 安装节点 install
    DESTINATION lib/${PROJECT_NAME}
)

