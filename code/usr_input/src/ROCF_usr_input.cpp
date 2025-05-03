#include "ROCF_usr_input.hpp"


ROCF_usr_input::ROCF_usr_input()
: ROCF_usr_input("usr_input")
{
}

// name is designed for different usr_inputter
ROCF_usr_input::ROCF_usr_input(const std::string & name)
: name_(name) 
{
}

ROCF_usr_input::~ROCF_usr_input() {}


void ROCF_usr_input::pubROS2Topic_ROCF(
  const std::string & topic,
  const std::string & msgType,
  const YAML::Node & msg_yaml,
  const std::string prefix_source)
{
  const std::string CommandWithResult = pubROS2Topic(topic, msgType, msg_yaml,prefix_source);
  logger_.log(CommandWithResult);
  return ; 
}

void ROCF_usr_input::pubROS2Action_ROCF(
  const std::string & action,
  const std::string & actionType,
  const YAML::Node & msg_yaml,
  const std::string prefix_source)
{
  const std::string CommandWithResult = pubROS2Action(action, actionType, msg_yaml, prefix_source);
  logger_.log(CommandWithResult);
  return ;
}

void ROCF_usr_input::pubROS2Service_ROCF(
  const std::string & service,
  const std::string & serviceType,
  const YAML::Node & msg_yaml,
  const std::string prefix_source)
{
  const std::string CommandWithResult = pubROS2Service(service, serviceType, msg_yaml,prefix_source);
  logger_.log(CommandWithResult);
     /* @todo Fuzz-Attack based on ROS2-Service
      * use mutate(yaml) as msg_data
      * send command : ros2 service call {service_name} {serviceType} "{msg_data}"
      * monitor: check if service successful, if true, it will be recorded
      * then, Attacker can use recorded command to attack any machine within the same service
      */
  return ;
}

void 
ROCF_usr_input::pubROS2DynamicParam_ROCF(
  const std::string & node_name,
  const std::string & param_name,
  const std::string & param_value)
{
  const std::string CommandWithResult = pubROS2DynamicParam(node_name,param_name,param_value);
  logger_.log(CommandWithResult);

  return ; 
}

void 
ROCF_usr_input::pubROS2Lifecyle_ROCF(
  const std::string & node_name,
  const std::string & operation, // 0 = "set", 1 = "get"
  const std::string & lifecycle_value)
{
  const std::string CommandWithResult = pubROS2Lifecyle(node_name, operation, lifecycle_value);
  logger_.log(CommandWithResult);
  return ;
}
