#ifndef __LOGGER_H__
#define __LOGGER_H__
#include <fstream>
#include <iostream>
#include <mutex>

class Logger {
private:
  Logger();
  ~Logger();
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

private:
  const std::string logFile = "proxy.log";
  static std::ofstream out;
  std::mutex m;

public:
  static Logger &getLogger() {
    static Logger logger;
    return logger;
  }
  /*
   * Log out message
   */
  void log(const std::string &msg);

  /*
   * Log out in proxy format, thread safe keep by mutex
   * @id proxy assigned request id
   * @msg log information
   */
  void proxyLog(const int id, const std::string &msg);
};

#endif
