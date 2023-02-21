#include "ResponseFactory.h"

Response ResponseFactory::create_200_ok_response() {
  unsigned int version = 11;
  std::string status_code = "200";
  std::string reason = "OK";
  std::map<std::string, std::string> header;
  std::string body;

  return Response(version, status_code, reason, header, body);
}

Response ResponseFactory::create_4xx_response(const std::string &status_code,
                                              const std::string &reason) {
  unsigned int version = 11;
  std::map<std::string, std::string> header;
  std::string body;

  return Response(version, status_code, reason, header, body);
}
