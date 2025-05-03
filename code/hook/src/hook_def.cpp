#include "hook_def.hpp"

const char *SERVER_ADDRESS = "127.0.0.1";
int sock = 0;
struct sockaddr_in serv_addr;
bool SOCKET_CONNECTION_SUCCESS = false;


void deserialize_CallStackInfo(const char *buffer, CallStackInfo &data) {
    std::memcpy(&data.start_time, 
        buffer, 
        sizeof(data.start_time));
    std::memcpy(&data.end_time, 
        buffer + sizeof(data.start_time), 
        sizeof(data.end_time));
    std::memcpy(&data.func_hash, 
        buffer + sizeof(data.start_time) + sizeof(data.end_time) , 
        sizeof(data.func_hash));
    std::memcpy(&data.thread_id, 
        buffer + sizeof(data.start_time) + sizeof(data.end_time)+sizeof(data.func_hash) , 
        sizeof(data.thread_id));
}
bool initialize_client() {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Socket creation error\n");
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address / Address not supported\n");
        return false;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Connection Failed\n");
        return false;
    }

    return true;
}