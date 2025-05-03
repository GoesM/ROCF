#ifndef USR_INPUT_H
#define USR_INPUT_H

#include <yaml-cpp/yaml.h>
#include "new_popen.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <csignal>

class usr_input
{
public:
  usr_input() {}
  virtual void update()
  {
    shutdown();
    usr_input_exit_flag_ = false;
  }
  void shutdown()
  {
    usr_input_exit_flag_ = true;
    shutdown_command_();
    if (thread_.joinable()) {
      thread_.join();
    }
    // std::cout << "[debug]:user_process shutdown()" << std::endl;
  }
  ~usr_input() {}

public:
  std::string pubROS2Topic(
    const std::string & topic,
    const std::string & msgType,
    const YAML::Node & msg_yaml,
    const std::string prefix_source = "");

  std::string pubROS2Action(
    const std::string & action,
    const std::string & actionType,
    const YAML::Node & msg_yaml,
    const std::string prefix_source = "");

  std::string pubROS2Service(
    const std::string & service,
    const std::string & serviceType,
    const YAML::Node & msg_yaml,
    const std::string prefix_source = "");

  std::string pubROS2DynamicParam(
    const std::string & node_name,
    const std::string & param_name,
    const std::string & param_value
  );

  std::string pubROS2Lifecyle(
    const std::string & node_name,
    const std::string & operation, // 0 = "set", 1 = "get"
    const std::string & lifecycle_value
  );

protected:
  std::thread thread_;


protected:
  std::string publishToROS2Topic(
    const std::string & topic,
    const std::string & msgType,
    const YAML::Node & msg_yaml,
    const std::string & prefix_source);

  std::string publishROS2Action(
    const std::string & action,
    const std::string & actionType,
    const YAML::Node & msg_yaml,
    const std::string & prefix_source);

  std::string publishROS2Service(
    const std::string & service,
    const std::string & serviceType,
    const YAML::Node & msg_yaml,
    const std::string & prefix_source);
  
  std::string setROS2DynamicParam(
    const std::string & node_name,
    const std::string & param_name,
    const std::string & param_value
  );

  std::string setROS2Lifecyle(
    const std::string & node_name,
    const std::string & operation, // 0 = "set", 1 = "get"
    const std::string & lifecycle_value
  );

  std::string getCommandBack(const char * command);
  CommandProcess::SharedPtr sub_proc_{nullptr};
  void shutdown_command_();
  bool command_exit_flag_{false};


protected:
  bool usr_input_exit_flag_{false};
  std::mutex shutdown_lock_;

};

#endif
