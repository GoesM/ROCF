#ifndef QUEUE_USR_INPUT_H
#define QUEUE_USR_INPUT_H

#include "ROCF_usr_input.hpp"
#include "yaml_file.hpp"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
/*
transform YAML::Node into command_queue
do following command_queue
*/

class queue_usr_input : public ROCF_usr_input
{
public:
  // default for usr_input.log
  queue_usr_input();
  // name is designed for different usr_inputter
  queue_usr_input(const std::string & name);
  ~queue_usr_input();
  void update() override
  {
    shutdown();
    usr_input_exit_flag_ = false;
    finished_=true;
  }


  void launch(const YAML::Node & command_queue, const int max_command = 1000);
  void join();
  bool finished();


private:
  YAML::Node command_queue_;
  void do_queue(const YAML::Node & command_queue, const int max_command);
  bool finished_{true};

private:
  // same as ROCF_usr_input
  // ROCF_logger logger_{ROCF_logger(false)};
  // std::string name_{"usr_input"};
  // std::string log_path_{};
};


#endif
