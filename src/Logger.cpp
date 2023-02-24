#include "Logger.h"

std::ofstream Logger::out;

Logger::Logger() {
  out.open(logFile, std::ios::app);
}

Logger::~Logger() {
  out.close();
}

void Logger::log(const std::string &msg) {
  m.lock();
  out << msg << std::endl;
  m.unlock();
}

void Logger::proxyLog(const int id, const std::string &msg) {
  std::string message = std::to_string(id) + ": " + msg;
  log(message);
}

