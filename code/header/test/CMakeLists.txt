# test----yaml
add_executable(test_yaml_helper
    test_yaml_helper.cpp
)
target_link_libraries(test_yaml_helper
    PRIVATE
    header
    yaml-cpp
)
install(TARGETS  
    test_yaml_helper  # 安装节点 install
    DESTINATION lib/${PROJECT_NAME}
)

# test---- logger
add_executable(test_logger
    test_logger.cpp
)
target_link_libraries(test_logger
    PRIVATE
    header
    yaml-cpp
)
install(TARGETS  
    test_logger  # 安装节点 install
    DESTINATION lib/${PROJECT_NAME}
)


# test---- configer
add_executable(test_configer
    test_configer.cpp
)
target_link_libraries(test_configer
    PRIVATE
    header
    yaml-cpp
)
install(TARGETS  
    test_configer  # 安装节点 install
    DESTINATION lib/${PROJECT_NAME}
)




# test---- multi_process_manager
add_executable(test_new_popen
    test_new_popen.cpp
)
target_link_libraries(test_new_popen
    PRIVATE
    header
    yaml-cpp
)
install(TARGETS  
test_new_popen  # 安装节点 install
    DESTINATION lib/${PROJECT_NAME}
)

