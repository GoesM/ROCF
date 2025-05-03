add_executable(test_hook
    fuzzer_simulation.cpp
)# 添加源文件和头文件目录
target_link_libraries(test_hook
    PRIVATE
    ROCF_hook

    yaml-cpp
    jsoncpp
)

install(TARGETS 
  test_hook  # 安装节点 install
  DESTINATION lib/${PROJECT_NAME}
)
