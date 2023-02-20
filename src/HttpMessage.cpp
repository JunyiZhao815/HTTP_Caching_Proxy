#include "HttpMessage.h"

HttpMessage::HttpMessage(const unsigned int version,
                         std::map<std::string, std::string> &header,
                         std::string &body)
    : version(version), header(header), body(body) {}

std::string HttpMessage::header2string() const {
  std::string result;
  std::map<std::string, std::string>::const_iterator iter;
  for (iter = header.begin(); iter != header.end(); ++iter) {
    result += iter->first + ": " + iter->second + "\r\n";
  }
  return result;
}

std::string HttpMessage::getVersion() const {
  int major_version = version / 10;
  int minor_version = version % 10;
  std::stringstream ss;
  ss << "HTTP/" << major_version << "." << minor_version;
  return ss.str();
}
