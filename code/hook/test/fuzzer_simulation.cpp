#include "hook_communicator.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

inline void launch_target_program() {
    pid_t pid = fork();

    if (pid == -1) {
        // fork failed
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        // Redirect stdout and stderr to /dev/null to suppress output
        int fd = open("/dev/null", O_WRONLY);
        if (fd == -1) {
            perror("open /dev/null failed");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 stdout failed");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDERR_FILENO) == -1) {
            perror("dup2 stderr failed");
            exit(EXIT_FAILURE);
        }

        close(fd);

        // Execute the target program
        execl("./cs.exe", "./cs.exe", (char *) NULL);

        // If execl returns, it must have failed
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cout << "Child terminated by signal " << WTERMSIG(status) << std::endl;
        } else {
            std::cout << "Child terminated abnormally" << std::endl;
        }
    }
}


int main(){
    YAML::Node result;

    YAML::Node delayer_queue_;
    // 将 delayer_queue_ 赋值
    for (int i = 0; i < 10; ++i) {
        // delayer_queue_.push_back(0);
        delayer_queue_.push_back(i*1000000);
        // delayer_queue_.push_back(1000000);
    }
    
    // fuzzer
    HookCommunicator con_cov_analyzer;
    con_cov_analyzer.launch();
    int test_round = 10000;
    while(test_round--)
    {
        std::cout << "----------------left test time: " << test_round << std::endl;
        con_cov_analyzer.update(delayer_queue_);        
        launch_target_program();

        con_cov_analyzer.analysis();

        std::cout << "total_con_cov:      " << con_cov_analyzer.total_cov_result() << std::endl;
        std::cout << "con_cov_up_:        " << con_cov_analyzer.cov_up_result() << std::endl;
        result.push_back(con_cov_analyzer.total_cov_result());
    }
    con_cov_analyzer.shutdown();

    // 将 result 写入 YAML 文件
    std::ofstream fout("result.yaml");
    fout << result;
    fout.close();

    return 0;
}








