#ifndef __HTTP_MESSAGE_H__
#define __HTTP_MESSAGE_H__
#include <iostream>
#include <map>

class HttpMessage {
protected:
  std::map<std::string, std::string> header;
  std::string body;

public:
  HttpMessage(std::map<std::string, std::string> &header, std::string &body);
};

#endif
