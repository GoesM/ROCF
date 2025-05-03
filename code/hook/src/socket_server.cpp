#include "socket_server.hpp"

void SocketServer::shutdown_server(){
    server_exit_flag_ = true;
    std::cout << "Server exiting..." << std::endl;
    initialize_client();        // send msg to make sure exit 
    // thread.join()
    if (excutor_thread_.joinable()) {
        excutor_thread_.join();
    }
    close(server_fd);
    server_fd = -1;
}
void SocketServer::shutdown_client(){
    client_exit_flag_ = true;
    std::cout << "Client exiting..." << std::endl;
    //  std::cout << "Client number: " << client_sockets_.size() << std::endl;
    for (std::thread &t : client_threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
    for (int &t : client_sockets_){
        close(t);
    }
    client_threads_.clear();
    client_sockets_.clear();
}


void SocketServer::start_server() {
    int addrlen = sizeof(address);
    char buffer[sizeof(CallStackInfo)] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (!server_exit_flag_) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            break;
        }

        std::lock_guard<std::mutex> lock(queue_mutex_);
        client_threads_.emplace_back(&SocketServer::handle_client, this, new_socket);
        client_sockets_.emplace_back(new_socket);

    }

    close(server_fd);
}

void SocketServer::handle_client(int client_socket) {
    char buffer[sizeof(CallStackInfo)] = {0};
    while (!client_exit_flag_) {
        int valread = read(client_socket, buffer, sizeof(CallStackInfo));
        if (valread > 0) {
            CallStackInfo data;
            deserialize_CallStackInfo(buffer, data);
            // restore 接收到的数据
            std::lock_guard<std::mutex> lock(queue_mutex_);
            info_queue_.push_back(data);

        } else {
            break;
        }
    }
    close(client_socket);
}
