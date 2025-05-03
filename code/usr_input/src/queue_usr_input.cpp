#include "queue_usr_input.hpp"
#include <iostream>

queue_usr_input::queue_usr_input()
: ROCF_usr_input() {}

queue_usr_input::queue_usr_input(const std::string & name)
: ROCF_usr_input(name) {}

queue_usr_input::~queue_usr_input() {}

#include <thread>
void queue_usr_input::launch(const YAML::Node & command_queue, const int max_command)
{
  if(!command_queue.size()){
    finished_ = true;
    return ; 
  }
  command_queue_ = command_queue;
  log_path_ = read_env<std::string>("USR_INPUT_LOG_DIR") + "/" + name_ + ".log";
  logger_.updateLogPath(log_path_);
  logger_.log(name_ + ": launched in a thread.");
  finished_ = false;

  thread_ =  std::thread(&queue_usr_input::do_queue, this, command_queue_,max_command);
  return ; 
}

void queue_usr_input::join()
{
  if(thread_.joinable()) thread_.join();
  return ; 
}
bool queue_usr_input::finished()
{
  return finished_;
}

void queue_usr_input::do_queue(const YAML::Node & command_queue, const int max_command)
{
  log_path_ = read_env<std::string>("USR_INPUT_LOG_DIR") + "/" + name_ + ".log";
  logger_.updateLogPath(log_path_);

  logger_.log(name_ + " user_input launched!");
  std::cout << name_ << " user_input launched!" << std::endl;

  if(!command_queue.IsSequence())
  {
    logger_.unmute();
    logger_.error(name_ + "user_input recived a wrong command_queue-yaml, check it!");
    logger_.mute();
    return ;   
  }

  
  const int len = command_queue.size(); // make sure command_queue is a YAML::List
  int index = 0;
  for (index = 0; index < len && index < max_command; index++) {
    if (usr_input_exit_flag_) {break;}

    std::cout << name_ << " try to send a command " << std::endl;

    if (command_queue[index]["type"]["A-S-T"].as<std::string>() == "topic")
    {
      const std::string prefix_source = "source " + teststation_dir_ + "/install/setup.bash";
      pubROS2Topic_ROCF( // do command & log
        command_queue[index]["type"]["name"].as<std::string>() ,   // topic name
        command_queue[index]["type"]["type"].as<std::string>() ,   // topic type
        command_queue[index]["data"], // topic yaml
        prefix_source); 
      // std::this_thread::sleep_for(std::chrono::seconds(0.5)); // gap for topic
    }
    else if (command_queue[index]["type"]["A-S-T"].as<std::string>() == "action")
    {
      const std::string prefix_source = "source " + teststation_dir_ + "/install/setup.bash";
      pubROS2Action_ROCF( // do command & log
        command_queue[index]["type"]["name"].as<std::string>() ,   // action name
        command_queue[index]["type"]["type"].as<std::string>() ,   // action type
        command_queue[index]["data"], // action yaml
        prefix_source);
      // no gap for action
    }
    else if (command_queue[index]["type"]["A-S-T"].as<std::string>() == "service")
    {
      const std::string prefix_source = "source " + teststation_dir_ + "/install/setup.bash";
      pubROS2Service_ROCF( // do command & log
        command_queue[index]["type"]["name"].as<std::string>() ,   // service name
        command_queue[index]["type"]["type"].as<std::string>() ,   // service type
        command_queue[index]["data"], // service yaml
        prefix_source);
      // no gap for service
    }
    else if (command_queue[index]["type"]["A-S-T"].as<std::string>() == "parameter")
    {
      pubROS2DynamicParam_ROCF(
        command_queue[index]["type"]["name"].as<std::string>() ,  // node name
        command_queue[index]["type"]["type"].as<std::string>() ,  // param_name
        command_queue[index]["data"].as<std::string>()            // param_value
        // we transform every type of param_value into string. to avoid type-fix-operation;
      );
      // no gap for param
    }
    else if (command_queue[index]["type"]["A-S-T"].as<std::string>() == "lifecycle")
    {
      pubROS2DynamicParam_ROCF(
        command_queue[index]["type"]["name"].as<std::string>() ,  // node name
        command_queue[index]["type"]["type"].as<std::string>() ,  // param_name
        command_queue[index]["data"].as<std::string>()            // param_value
        // we transform every type of param_value into string. to avoid type-fix-operation;
      );
      // no gap for param
    }

    else {
      logger_.unmute();
      logger_.error("wrong command type:" + command_queue[index]["type"].as<std::string>());
      logger_.mute();
      continue;
    }
  }
  
  logger_.unmute();
  if (index > len) {logger_.log(name_ + " all usr-input queue have been done.");}
  logger_.log(name_ + " exit.");
  logger_.mute();

  finished_ = true;
  return ;

}
