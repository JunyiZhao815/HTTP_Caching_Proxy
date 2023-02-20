#ifndef __RESPONSE_H__
#define __RESPONSE_H__
#include "HttpMessage.h"

class Response : public HttpMessage {
private:
  const std::string status_code;
  const std::string reason;

public:
  Response(const unsigned int version, const std::string &status_code,
           const std::string &reason,
           std::map<std::string, std::string> &header, std::string &body);

  friend std::ostream &operator<<(std::ostream &out, const Response response);
};
#endif
