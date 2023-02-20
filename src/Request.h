#ifndef __REQUEST_H__
#define __REQUEST_H__
#include "HttpMessage.h"
#include <sstream>

class Request : public HttpMessage {
private:
  const std::string method;
  const std::string uri;

public:
  Request(const std::string &method, const std::string &uri,
          const unsigned int version,
          std::map<std::string, std::string> &header, std::string &body);

  friend std::ostream &operator<<(std::ostream &out, const Request request);

  /*
   * Get target host ip and port
   * @return <ip, port>, if not specify then assume port = 80
   */
  std::pair<std::string, std::string> getHost();
};

#endif
