#include "logger.hpp"

using namespace Time_ROCF;
using namespace Color;
using namespace web::websockets::client;

void Logger::open_FileMode(const std::string & fp ){   
    log_fp_ = fp;
}
void Logger::close_FileMode(){
    log_fp_ = "";
}
void Logger::open_terminal_mode(){
    terminal_mode_ = true;
}
void Logger::close_terminal_mode(){
    terminal_mode_ = false;
}

// 打开 WebMode
void Logger::open_WebMode(const std::string &host, const int & port) {
    HOST = host;
    PORT = port;
    try {
        _websocket = std::make_shared<websocket_client>();
        _websocket->connect(U("ws://" + host + ":" + std::to_string(port))).wait();
        web_mode_ = true;
        light("WebSocket connection established.", true);
    } catch (const std::exception &e) {
        error("Failed to connect to WebSocket - " + std::string(e.what()), true);
        web_mode_ = false;
    }
}
// 关闭 WebMode
void Logger::close_WebMode() {
    if (_websocket) {
        _websocket->close().wait();
        _websocket.reset();
        web_mode_ = false;
    }
}

// 日志打印函数
void Logger::log(const std::string &message, bool only_local) {
    _process_log("INFO", message, only_local);
}

void Logger::debug(const std::string &message) {
    _process_log("DEBUG", message, true);
}

void Logger::error(const std::string &message, bool only_local) {
    _process_log("ERROR", message, only_local);
}

void Logger::warn(const std::string &message, bool only_local) {
    _process_log("WARN", message, only_local);
}

void Logger::light(const std::string &message, bool only_local) {
    _process_log("LIGHT", message, only_local);
}

void Logger::title(const std::string &message, bool only_local) {
    std::string formatted_message = "================" + message + "================";
    _process_log("TITLE", formatted_message, only_local);
}

void Logger::subtitle(const std::string &message, bool only_local) {
    std::string formatted_message = "-----" + message + "-----";
    _process_log("subTITLE", formatted_message, only_local);
}

void Logger::quote(const std::string &message, bool only_local) {
    _process_log("QUOTE", message, only_local);
}

// 日志消息格式化
void Logger::_process_log(const std::string &level, const std::string &message, bool only_local) {
    // 本地日志格式化
    std::string timestamp = unix_stamp();  
    std::ostringstream oss;
    oss << "[" << timestamp << "][" << name_ << "][" << level << "]: " << message;
    std::string formatted_message = oss.str();

    // write terminal: 
    if(terminal_mode_)
    {
        std::cout << _color(formatted_message, level) << std::endl;
    }

    // 写入日志文件
    if (!log_fp_.empty()) {
        try {
            std::ofstream log_file(log_fp_, std::ios::app); // 以追加模式打开文件
            if (log_file.is_open()) {
                log_file << formatted_message << std::endl; // 写入日志
                log_file.close(); // 关闭文件
            } else {
                error("Failed to open log file: " + log_fp_ , true);
            }
        } catch (const std::exception &e) {
            error("Exception when writing to log file: " + std::string(e.what()), true);
        }
    }

    // write websocket:
    if (web_mode_ && !only_local) {
        try {
            _send_websocket_log({{"level", level}, {"message", formatted_message}});
        } catch (const std::exception &e) {
            error("Failed to send log via WebSocket - " + std::string(e.what()), true);
            web_mode_ = false;
        }
    }
}

// 根据日志级别设置颜色
std::string Logger::_color(const std::string &message, const std::string &level) {
    if (level == "QUOTE") {
        return gray(message);
    } else if (level == "subTITLE") {
        return bg_yellow(black(message));
    } else if (level == "TITLE") {
        return bg_blue(black(message));
    } else if (level == "LIGHT") {
        return purple(message);
    } else if (level == "WARN") {
        return yellow(message);
    } else if (level == "ERROR") {
        return red(message);
    } else if (level == "DEBUG") {
        return green(message);
    }
    return message;
}

// 发送 WebSocket 日志
void Logger::_send_websocket_log(const nlohmann::json &log_data) {
    if (_websocket) {
        web::websockets::client::websocket_outgoing_message message;
        message.set_utf8_message(log_data.dump());
        _websocket->send(message).wait();
    }
}
