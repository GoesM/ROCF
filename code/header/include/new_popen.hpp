#ifndef NEW_POPEN_HPP
#define NEW_POPEN_HPP
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <cstdlib>
#include <thread>
#include <future>
#include <memory>

namespace CommandProcess
{

class command_process
{
public:
  command_process();
  ~command_process();

  void new_popen(
    const std::string & command,
    bool if_drop_stdout_ = false,
    bool if_drop_stderr_ = false
  );
  void join();
  void shutdown();
  void terminate() {shutdown();}
  std::string get_output()
  {
    join();
    return std::string(result_);
  }
  bool if_finished() {return !is_child_alive();}

private:
  pid_t main_pid;
  size_t pid_size_ = sizeof(pid_t);
  pid_t * child_pid_ = (pid_t *)mmap(
    NULL, pid_size_, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
    -1, 0);

  size_t result_size_ = 1e7;   // for string
  char * result_ = (char *)mmap(
    NULL, result_size_, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
    -1, 0);

private:
  void wait_child_clean();
  bool is_child_alive();

private:
  // static void signal_handler(int signum);
  void do_command(
    const std::string & command, bool if_drop_stdout_ = false,
    bool if_drop_stderr_ = false);
};


using SharedPtr = std::shared_ptr<command_process>;
}


CommandProcess::SharedPtr
goes_popen(
  const std::string & command,
  bool if_drop_stdout_,
  bool if_drop_stderr_);

#endif // command_process_HPP
