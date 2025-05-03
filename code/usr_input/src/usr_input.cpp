#include "usr_input.hpp"

std::string usr_input::pubROS2Topic(
  const std::string & topic,
  const std::string & msgType,
  const YAML::Node & msg_yaml,
  const std::string prefix_source)
{
  return publishToROS2Topic(topic, msgType, msg_yaml,prefix_source);
}

std::string usr_input::pubROS2Action(
  const std::string & action,
  const std::string & actionType,
  const YAML::Node & msg_yaml,
  const std::string prefix_source)
{
  return publishROS2Action(action, actionType, msg_yaml,prefix_source);
}

std::string usr_input::pubROS2Service(
  const std::string & service,
  const std::string & serviceType,
  const YAML::Node & msg_yaml,
  const std::string prefix_source)
{
  return publishROS2Service(service, serviceType, msg_yaml,prefix_source);
}

std::string usr_input::pubROS2DynamicParam(
    const std::string & node_name,
    const std::string & param_name,
    const std::string & param_value
  )
{
  return setROS2DynamicParam(node_name,param_name,param_value);
}

std::string usr_input::pubROS2Lifecyle(
    const std::string & node_name,
    const std::string & operation, // 0 = "set", 1 = "get"
    const std::string & lifecycle_value
  )
{
  return setROS2Lifecyle(node_name,operation,lifecycle_value);
}



std::string usr_input::publishToROS2Topic(
  const std::string & topic,
  const std::string & msgType,
  const YAML::Node & msg_yaml,
  const std::string & prefix_source)
{
  std::string yamlString = YAML::Dump(msg_yaml);

  std::stringstream ss;
  if( !prefix_source.empty() ) { ss << prefix_source << " && ";}
  ss << "ros2 topic pub " << topic << " " << msgType << " \" \n";
  ss << yamlString;
  ss << "\" -1";

  std::string command = ss.str();

  std::string result = getCommandBack(command.c_str());

  return "command: \n" + command + "\n" + "result: \n" + result;
}

std::string usr_input::publishROS2Action(
  const std::string & action,
  const std::string & actionType,
  const YAML::Node & msg_yaml,
  const std::string & prefix_source)
{
  std::string yamlString = YAML::Dump(msg_yaml);

  std::stringstream ss;
  if( !prefix_source.empty() ) { ss << prefix_source << " && ";}
  ss << "ros2 action send_goal " << action << " " << actionType << " \" \n";
  ss << yamlString;
  ss << "\" ";

  std::string command = ss.str();

  // std::cout << "[DEBUG]: do command \n" << command << std::endl;

  std::string result = getCommandBack(command.c_str());

  return "command: \n" + command + "\n" + "result: \n" + result;
}

std::string usr_input::publishROS2Service(
  const std::string & service,
  const std::string & serviceType,
  const YAML::Node & msg_yaml,
  const std::string & prefix_source)
{
  std::string yamlString = YAML::Dump(msg_yaml);

  std::stringstream ss;
  if( !prefix_source.empty() ) { ss << prefix_source << " && ";}
  ss << "ros2 service call " << service << " " << serviceType << " \" \n";
  ss << yamlString;
  ss << "\" ";

  std::string command = ss.str();

  std::string result = getCommandBack(command.c_str());

  return "command: \n" + command + "\n" + "result: \n" + result;
}

std::string 
usr_input::setROS2DynamicParam(
    const std::string & node_name,
    const std::string & param_name,
    const std::string & param_value
  )
{
  std::stringstream ss;
  ss << "ros2 param set " << node_name << " " << param_name << " " << param_value;

  std::string command = ss.str();

  std::string result = getCommandBack(command.c_str());

  return "command: \n" + command + "\n" + "result: \n" + result;
}

std::string 
usr_input::setROS2Lifecyle(
    const std::string & node_name,
    const std::string & operation, // 0 = "set", 1 = "get"
    const std::string & lifecycle_value
  )
{
  std::stringstream ss;
  ss << "ros2 lifecyle set " << node_name << " " << operation << " " << lifecycle_value;

  std::string command = ss.str();

  std::string result = getCommandBack(command.c_str());

  return "command: \n" + command + "\n" + "result: \n" + result;
}


std::string usr_input::getCommandBack(const char * command)
{
  shutdown_command_();
  if (usr_input_exit_flag_) {return "";}

  std::string command_(command);
  sub_proc_ = goes_popen(
    command_, true, true
  );
  std::string result = "";
  command_exit_flag_ = false;
  while ((!sub_proc_->if_finished()) && (!command_exit_flag_) && (!usr_input_exit_flag_)) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
  }
  if (sub_proc_->if_finished()) {result = sub_proc_->get_output();} else {
    result = "Interrupt by exit_flag_";
  }
  return result;
}

void usr_input::shutdown_command_()
{
  shutdown_lock_.lock();

  command_exit_flag_ = true;
  if (sub_proc_ != nullptr) {
    // std::cout << "[debug]:shutdown command in usr-inpupt [start]" << std::endl;
    sub_proc_->shutdown();
    sub_proc_->join();
    sub_proc_.reset();
    // std::cout << "[debug]:shutdown command in usr-inpupt [end]" << std::endl;
  } else {
    // std::cout << "[debug]: sub_proc has been cleaned" << std::endl;
  }

  shutdown_lock_.unlock();
}
