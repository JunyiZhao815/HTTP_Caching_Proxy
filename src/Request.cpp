#include "Request.h"

Request::Request(const std::string &version, const std::string &status_code,
                 const std::string &reason,
                 std::map<std::string, std::string> &header, std::string &body)
    : HttpMessage(header, body), version(version), status_code(status_code),
      reason(reason) {}
