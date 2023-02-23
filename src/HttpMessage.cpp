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
    std::string errmsg = "400|No " + name + " field in head";
    throw std::invalid_argument(errmsg);
  }
}

std::string HttpMessage::getCacheControl() {
  checkHasField("cache-control");
  return header["cache-control"];
}

std::string HttpMessage::getMaxAge() {
  std::string cache_control = getCacheControl();
  size_t index = cache_control.find("max-age=");
  if (index != std::string::npos) {
    index += 8;
    std::string ans = "";
    while (isdigit(cache_control[index])) {
      ans += cache_control[index];
      index++;
    }
    header["max-age"] = ans;
  }
  return header["max-age"];
}

std::string HttpMessage::getDate() {
  checkHasField("date");
  return header["date"];
}

bool HttpMessage::operator==(HttpMessage &httpMessage) {
  std::string a = message2string();
  std::string b = httpMessage.message2string();
  if (a == b) {
    return true;
  }
  return false;
}

std::string HttpMessage::getHeaderField(std::string name) {
  checkHasField(name);
  return header[name];
}

HttpMessage::~HttpMessage() {}

bool HttpMessage::doesHeaderHave(std::string name) {
  if (header.find(name) == header.end()) {
    return false;
  }
  return true;
}

void HttpMessage::addHeaderField(const std::string &name,
                                 const std::string &value) {
  if (header.find(name) != header.end()) {
    header[name] = value;
  } else {
    std::pair<std::string, std::string> key_value;
    key_value.first = name;
    key_value.second = value;
    header.insert(key_value);
  }
}

void HttpMessage::removeHeaderField(std::string name){
  if(header.find(name) != header.end()){
    header.erase(name);
  }
}

size_t HttpMessage::getMessageLen(){
  return message2string().length();
}