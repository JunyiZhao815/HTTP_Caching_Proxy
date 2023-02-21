#include "Request.h"

Request::Request(const std::string &method, const std::string &uri,
                 const unsigned int version,
                 std::map<std::string, std::string> &header, std::string &body)
    : HttpMessage(version, header, body), method(method), uri(uri) {}

std::ostream &operator<<(std::ostream &out, const Request request) {
  // start line
  out << request.method << " " << request.uri << " " << request.getVersion()
      << "\r\n";
  // header
  out << request.header2string();
  // blank line
  out << "\r\n";
  // body
  out << request.body;
  return out;
}

std::pair<std::string, std::string> Request::getHost() {
  std::string host = header["host"];
  std::pair<std::string, std::string> ip_port;
  size_t index = host.find(':');
  if (index == std::string::npos) {
    ip_port.first = host;
    ip_port.second = "80";
    return ip_port;
  } else {
    ip_port.first = host.substr(0, index);
    ip_port.second = host.substr(index + 1);
    return ip_port;
  }
}

std::string Request::message2string() {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

std::string Request::getMethod() { return method; }

std::string Request::getUrl() { return uri; }
