#include "Response.h"

Response::Response(const std::string &method, const std::string &uri,
                   const std::string &version,
                   std::map<std::string, std::string> &header,
                   std::string &body)
    : HttpMessage(header, body), method(method), uri(uri), version(version) {}
