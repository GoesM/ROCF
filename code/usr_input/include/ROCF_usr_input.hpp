#ifndef ROCF_USR_INPUT_H
#define ROCF_USR_INPUT_H

#include "global_env.hpp"
#include "ROCF_logger.hpp"
#include "usr_input.hpp"
#include <string>

class ROCF_usr_input : public usr_input
{
public:
  ROCF_usr_input();
  ROCF_usr_input(const std::string & name);
  void setup()
  {
    teststation_dir_ = read_env<std::string>("TESTSTATION");
  }
  ~ROCF_usr_input();

  void pubROS2Topic_ROCF(
    const std::string & topic,
    const std::string & msgType,
    const YAML::Node & msg_yaml,
    const std::string prefix_source = "");

  void pubROS2Action_ROCF(
    const std::string & action,
    const std::string & actionType,
    const YAML::Node & msg_yaml,
    const std::string prefix_source = "");

  void pubROS2Service_ROCF(
    const std::string & service,
    const std::string & serviceType,
    const YAML::Node & msg_yaml,
    const std::string prefix_source = "");

  void pubROS2DynamicParam_ROCF(
    const std::string & node_name,
    const std::string & param_name,
    const std::string & param_value
  );

  void pubROS2Lifecyle_ROCF(
    const std::string & node_name,
    const std::string & operation, // 0 = "set", 1 = "get"
    const std::string & lifecycle_value
  );

  

protected:
  ROCF_logger logger_{ROCF_logger(false)};
  std::string name_{"usr_input"};
  std::string log_path_{""};
  std::string teststation_dir_{""};
};

#endif
