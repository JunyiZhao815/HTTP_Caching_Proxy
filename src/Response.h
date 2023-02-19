#ifndef __RESPONSE_H__
#define __RESPONSE_H__
#include "HttpMessage.h"

class Response : public HttpMessage {
private:
  const std::string method;
  const std::string uri;
  const std::string version;

public:
  Response(const std::string &method, const std::string &uri,
           const std::string &version,
           std::map<std::string, std::string> &header, std::string &body);
};
#endif
