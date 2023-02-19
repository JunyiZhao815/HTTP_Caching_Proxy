#include "HttpMessage.h"

HttpMessage::HttpMessage(std::map<std::string, std::string> &header,
                         std::string &body)
    : header(header), body(body) {}
