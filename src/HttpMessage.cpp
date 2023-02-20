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

void HttpMessage::checkHasField(const std::string &name) const {
  if (header.find(name) == header.end()) {
    std::string errmsg = "No " + name + " field in head";
    throw std::invalid_argument(errmsg);
  }
}

std::string HttpMessage::getCacheControl() {
  checkHasField("cache-control");
  return header["cache-control"];
}

std::string HttpMessage::getMaxAge() {
  checkHasField("max-age");
  return header["max-age"];
}

bool HttpMessage::operator==(HttpMessage &httpMessage) {
  std::string a = message2string();
  std::string b = httpMessage.message2string();
  if (a == b) {
    return true;
  }
  return false;
}

HttpMessage::~HttpMessage() {}
