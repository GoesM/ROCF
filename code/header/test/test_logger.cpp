// test_logger.cpp

#include "logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

// 使用命名空间以简化代码
using namespace std;

// 测试本地日志记录功能
void TEST_LocalLogger() {
    cout << "===== TEST_LocalLogger =====" << endl;

    Logger logger = Logger("TEST_LOGGER");

    // 打开文件日志模式（可选）
    string log_file = "local_log.txt";
    logger.open_FileMode(log_file);
    cout << "File logging enabled: " << log_file << endl;

    // 打开终端日志模式（默认已启用，可选）
    logger.open_terminal_mode();
    cout << "Terminal logging enabled." << endl;

    // 记录各种类型的日志
    logger.title("This is a title message.");
    this_thread::sleep_for(chrono::milliseconds(100)); // 延时以区分日志时间
    logger.subtitle("This is a subtitle message.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.log("This is a normal log message.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.error("This is an error message.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.warn("This is a warning message.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.light("This is a light message.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.quote("This is a quote message.");
    this_thread::sleep_for(chrono::milliseconds(100));

    // 关闭文件日志模式
    logger.close_FileMode();
    cout << "File logging disabled." << endl;

    // 关闭终端日志模式（可选）
    logger.close_terminal_mode();
    cout << "Terminal logging disabled." << endl;

    cout << "===== END OF TEST_LocalLogger =====\n" << endl;
}

// 测试 WebSocket 日志记录功能
void TEST_WebLogger() {
    cout << "===== TEST_WebLogger =====" << endl;

    Logger logger = Logger("TEST_LOGGER");

    // 提示用户启动 WebSocket 服务器
    cout << "请确保已经在另一个终端启动了 WebSocket 服务器（例如运行 'python3 logger_server.py'）。" << endl;
    cout << "按回车键继续..." << endl;
    cin.ignore(); // 等待用户输入

    // 打开 WebSocket 模式
    string host = "127.0.0.1";
    int port = 1125; // 请确保服务器监听的端口与此处一致
    logger.open_WebMode(host, port);
    cout << "WebSocket logging enabled: ws://" << host << ":" << port << endl;
    // 打开文件日志模式（可选）
    string log_file = "local_log.txt";
    logger.open_FileMode(log_file);

    // 记录各种类型的日志
    logger.title("This is a title message sent to WebSocket.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.subtitle("This is a subtitle message sent to WebSocket.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.log("This is a normal log message sent to WebSocket.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.error("This is an error message sent to WebSocket.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.warn("This is a warning message sent to WebSocket.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.light("This is a light message sent to WebSocket.");
    this_thread::sleep_for(chrono::milliseconds(100));
    logger.quote("This is a quote message sent to WebSocket.");
    this_thread::sleep_for(chrono::milliseconds(100));

    // 关闭 WebSocket 模式
    logger.close_WebMode();
    cout << "WebSocket logging disabled." << endl;

    cout << "===== END OF TEST_WebLogger =====" << endl;
}

int main() {
    // 运行本地日志记录测试
    TEST_LocalLogger();

    // 运行 WebSocket 日志记录测试
    TEST_WebLogger();

    return 0;
}
