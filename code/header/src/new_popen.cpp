#include "new_popen.hpp"

CommandProcess::SharedPtr
goes_popen(
  const std::string & command,
  bool if_drop_stdout_,
  bool if_drop_stderr_)
{
  CommandProcess::SharedPtr commander_ptr =
    std::make_shared<CommandProcess::command_process>();
  commander_ptr->new_popen(command, if_drop_stdout_, if_drop_stderr_);
  return commander_ptr;
}

namespace CommandProcess
{

command_process::command_process()
{
  main_pid = getpid();
}
command_process::~command_process()
{
  if (getpid() != main_pid) {return;}
  shutdown();join();
  // 释放共享内存
  munmap(child_pid_, pid_size_);
  munmap(result_, result_size_);
}

void
command_process::new_popen(
  // the first fork
  const std::string & command,
  bool if_drop_stdout_,
  bool if_drop_stderr_)
{
  // fork
  pid_t pid = fork();
  if (pid == -1) {
    std::cerr << "fork() in new_popen() failed!" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {   // Child process
    // new pid group
    if (setsid() == -1) {          // 在子进程中调用 setsid() 来创建一个新的会话，并使子进程成为该会话的领导。
      perror("setsid");
      exit(EXIT_FAILURE);
    }
    // tell 子进程的进程组ID（pgid） to main-process
    *child_pid_ = getpid();
    do_command(command, if_drop_stdout_, if_drop_stderr_);

    // exit(0);
    execlp("/bin/true", "/bin/true", NULL);
    std::cerr << "execl() failed!" << std::endl;
    exit(EXIT_FAILURE);
  } else {   // Parent process
    // no block
    while (!(*child_pid_)) {usleep(10000);}
    // wait child change
  }
}

void
command_process::shutdown()
{
  if (!is_child_alive()) {return;}

  // 发送 shutdown信号给子进程
  // std::cout << "shutdown pid=" << *child_pid_ << std::endl;
  while (is_child_alive()) {
    kill(-(*child_pid_), SIGINT);
    kill(-(*child_pid_), SIGTERM);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return;
}


void
command_process::join()
{
  wait_child_clean();
}
void
command_process::wait_child_clean()
{
  if (*child_pid_ > 0) {
    int status;
    waitpid(-(*child_pid_), &status, 0);
  }
  // std::cout << "debug flag in: wait_child_clean(),after wait." << std::endl;
}
bool
command_process::is_child_alive()
{
  int status; pid_t ls;
  ls = waitpid(-(*child_pid_), &status, WNOHANG);
  if (ls == 0) {return true;}
  return false;
}


void
command_process::do_command(
  const std::string & command,
  bool if_drop_stdout_,
  bool if_drop_stderr_)
{
  std::string command_ = "";
  if (!if_drop_stderr_) {command_ = command;} else {command_ = command + " 2>/dev/null";}
  // pipe communication
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    std::cerr << "pipe() failed!" << std::endl;
    exit(EXIT_FAILURE);
  }
  // fork
  pid_t pid = fork();
  if (pid == -1) {
    std::cerr << "fork() failed!" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {   // Child process
    close(pipefd[0]);     // Close reading end of the pipe
    // Redirect stdout to the writing end of the pipe
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);     // Close duplicated writing end of the pipe

    // Execute the command
    execl("/bin/bash", "bash", "-c", command.c_str(), NULL);
    std::cerr << "execl() failed! check your command! " << std::endl;
    exit(EXIT_FAILURE);
  } else { // Parent process
    close(pipefd[1]);     // Close writing end of the pipe

    // Read command output from the reading end of the pipe
    std::string result = "";
    char buffer[128];
    ssize_t bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
      // read result
      result.append(buffer, bytesRead);

      // stdout
      if (!if_drop_stdout_) {
        // Output the buffer in real time
        std::cout.write(buffer, bytesRead);
        std::cout.flush();
      }
    }
    // Wait for the child process to finish
    int status;
    waitpid(pid, &status, 0);

    close(pipefd[0]);     // Close reading end of the pipe

    // 将结果复制到result_中
    std::strcpy(result_, result.c_str());
  }
}

}
