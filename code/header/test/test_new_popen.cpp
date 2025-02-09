#include "new_popen.hpp"


std::string command =
  R"(
ros2 topic pub /initialpose geometry_msgs/PoseWithCovarianceStamped "\
header:
    stamp:
        sec: 0,
        nanosec: 0
    frame_id: 'map'
pose:
    pose:
        position: 
            x: -2.0,
            y: -0.5,
            z: 0.0
        orientation:
            x: 0.0, 
            y: 0.0, 
            z: 0.01, 
            w: 1.0
    covariance: [0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.06853891909122467]
" -1 
)";
std::string command_1 = "ros2 topic echo /scan --full-length --once";
std::string command_2 = "ros2 topic list";


inline void a_gap()
{
  // it's just a gap
  for (int i = 0; i < 3; ++i) {
    std::cout << "Main thread is doing task " << i << std::endl;
    // 在这里添加其他任务代码
    std::this_thread::sleep_for(std::chrono::seconds(1));     // 模拟执行任务的耗时
  }
}
int main()
{
  // Introduction to API:

  // 1. create a commander:
  // 执行命令
  CommandProcess::SharedPtr proc_1 = goes_popen(
    command, false, false
  );
  CommandProcess::SharedPtr proc_2 = goes_popen(
    command_1, false, true
  );                  // drop the stderr
  CommandProcess::SharedPtr proc_3 = goes_popen(
    command_2, true, false
  );             // drop the stdout

  a_gap();

  // 2. terminate commander
  proc_1->terminate();

  // 3. block the main-proc
  proc_1->join();

  // 4. positive check if_finished()
  if (proc_1->if_finished()) {std::cout << "proc_1 has terminate" << std::endl;}
  if (proc_2->if_finished()) {std::cout << "proc_2 has terminate" << std::endl;} else {
    std::cout << "proc_2 is still working" << std::endl;
  }
  // 5. get the output from the command
  std::string result = proc_3->get_output();   // it will block the main-proc, until command finished
  std::cout << "read from proc_3: \n" << result << std::endl;

  // 6. how to get the result with no block?
  // OR how to terminate during the result reading?
  //    use structure like following:
  // Firstly, you should provide one flag:
  bool exit_flag_ = false;
  // Then, early shutdown module
  // you should read result after ->if_finished() is true
  while (!proc_2->if_finished()) {
    a_gap();
    // change exit_flag_ = true when you need to pre-shutdown proc_1
    {
      exit_flag_ = true; // it will cancel your command
    }
    if (exit_flag_ == true) {
      proc_2->terminate();
      proc_2->join();
      break;
    }
  }
  // [important!]: if you break the command by terminate(), the result would be empty
  std::cout << "read from proc_2: " << proc_2->get_output() << std::endl;
  // [notice]: you can only get the output after finished
  // if you early shutdown the command, you would read nothing.

  proc_1.reset();
  proc_2.reset();
  proc_3.reset();

  std::cout << "demo finished." << std::endl;
  return 0;
}
