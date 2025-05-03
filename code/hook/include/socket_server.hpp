#include "hook_def.hpp"


#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

class SocketServer {
private:
    // collected data
    std::vector<CallStackInfo> info_queue_;
    std::mutex queue_mutex_;
    // socket
    int server_fd;
    struct sockaddr_in address;
    // threads
    bool server_exit_flag_{false};
    bool client_exit_flag_{false};
    std::thread excutor_thread_;
    std::vector<std::thread> client_threads_;
    std::vector<int> client_sockets_;
    std::mutex exit_mutex_;

public:
    SocketServer() {}
    ~SocketServer() {
        if (server_fd >= 0) {
            shutdown();
        }
    }

    void update() { // for each round
        server_exit_flag_ = false;
        client_exit_flag_ = false;
        std::lock_guard<std::mutex> lock(queue_mutex_);
        info_queue_.clear();
        client_threads_.clear();
    }

    void launch() {
        std::cout << "Server launched" << std::endl;
        excutor_thread_ = std::thread(&SocketServer::start_server, this);
    }

    void clear(){ // for each round
        shutdown_client();
        return ; 
    }

    void shutdown() {
        std::lock_guard<std::mutex> lock(exit_mutex_);
        shutdown_client();
        shutdown_server();
        return ; 
    }

    std::vector<CallStackInfo> result(){
        return info_queue_;
    }

private:
    void shutdown_server();
    void shutdown_client();

private:
    void start_server();

    void handle_client(int client_socket);
};


#endif