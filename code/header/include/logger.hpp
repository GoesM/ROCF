#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>
#include <cpprest/ws_client.h>  // WebSocket 客户端库

namespace Time_ROCF{
// unix_stamp
std::string unix_stamp()
{
  auto timestamp = std::chrono::high_resolution_clock::now();
  auto seconds =
    std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
  auto fractional_seconds = std::chrono::duration_cast<std::chrono::microseconds>(
    timestamp.time_since_epoch()).count() % 1000000;

  // 格式化输出
  std::ostringstream oss;
  oss << seconds << "." << std::setw(6) << std::setfill('0') << fractional_seconds;

  return oss.str();
}

// year-month-day-hour-minute-second
std::string read_time()
{
  auto now = std::chrono::system_clock::now();
  auto time_t_now = std::chrono::system_clock::to_time_t(now);

  std::ostringstream oss;
  oss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d_%H-%M-%S");

  return oss.str();
}

};

namespace Color{
  // font_color
  const std::string red_font = "\033[91m";
  const std::string green_font = "\033[92m";
  const std::string yellow_font = "\033[93m";
  const std::string blue_font = "\033[94m";
  const std::string purple_font = "\033[95m";
  const std::string gray_font = "\033[90m";
  const std::string white_font = "\033[97m";
  const std::string black_font = "\033[30m";
  // background_color
  const std::string yellow_BG = "\033[48;5;226m";
  const std::string blue_BG = "\033[48;5;32m";
  const std::string white_BG = "\033[48;5;231m";
  const std::string black_BG = "\033[48;5;16m";
  // reset color
  const std::string color_reset = "\033[0m";

  std::string red(const std::string & text) {return red_font+text+color_reset;}
  std::string yellow(const std::string & text) {return yellow_font+text+color_reset;}
  std::string purple(const std::string & text) {return purple_font+text+color_reset;}
  std::string blue(const std::string & text) {return blue_font+text+color_reset;}
  std::string green(const std::string & text) {return green_font+text+color_reset;}
  std::string black(const std::string & text) {return black_font+text+color_reset;}
  std::string gray(const std::string & text) {return gray_font+text+color_reset;}
  std::string bg_black(const std::string & text) {return black_BG+text+color_reset;}
  std::string bg_white(const std::string & text) {return white_BG+text+color_reset;}
  std::string bg_blue(const std::string & text) {return blue_BG+text+color_reset;}
  std::string bg_yellow(const std::string & text) {return yellow_BG+text+color_reset;}
  
};


class Logger
{
private:
  std::string log_fp_{""}; // write into file 
  bool terminal_mode_{true};    // write into terminal
  bool web_mode_{false};        // send to web_logger

public:  // mode change
  Logger(const std::string & name)
  : name_(name) {}
  ~Logger() {}

  void open_FileMode(const std::string & fp);
  void close_FileMode();

  void open_terminal_mode();
  void close_terminal_mode();
  
  void open_WebMode(const std::string & host, const int & port);
  void close_WebMode();


public:  // logger

  void log  (const std::string & context, bool only_local = false);
  void debug(const std::string & context);
  void warn (const std::string & context, bool only_local = false);
  void light(const std::string & context, bool only_local = false);
  void error(const std::string & context, bool only_local = false);
  void title(const std::string & context, bool only_local = false);
  void subtitle(const std::string & context, bool only_local = false);
  void quote(const std::string & context, bool only_local = false);

private:
  void _process_log(const std::string &level, const std::string & context, bool only_local);
  std::string _color(const std::string &message, const std::string &level);
  void _send_websocket_log(const nlohmann::json & log_data);
  
private:
  std::string name_{"ROCF"};
  std::string HOST{"127.0.0.1"};
  int PORT{1125};
  std::shared_ptr<web::websockets::client::websocket_client> _websocket{nullptr};

};


#endif
