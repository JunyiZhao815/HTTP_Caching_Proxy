#ifndef __REQUEST_H__
#define __REQUEST_H__
#include "HttpMessage.h"

class Request : public HttpMessage {
private:
  const std::string version;
  const std::string status_code;
  const std::string reason;

public:
  Request(const std::string &version, const std::string &status_code,
          const std::string &reason, std::map<std::string, std::string> &header,
          std::string &body);
};

#endif
