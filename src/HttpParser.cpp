#include "HttpParser.h"

void HttpParser::addToRemainParsed(const char *msg, const size_t len) {
  for (size_t m = 0; m < len; m++) {
    remainParsed.push_back(msg[m]);
  }
}

Request *HttpParser::createRequest(http::request<http::string_body> message) {

  std::string method = message.method_string().to_string();
  std::string uri = message.target().to_string();

  std::map<std::string, std::string> request_header;
  collectHeaderKV<true>(message, request_header);

  std::stringstream ss;
  ss << message.body();
  std::string body = ss.str();

  Request *request =
      new Request(method, uri, message.version(), request_header, body);
  return request;
}

Request *HttpParser::parseRequest(const char *msg, const size_t len) {
  addToRemainParsed(msg, len);
  boost::system::error_code ec;
  size_t consumed = requestParser.put(
      boost::asio::buffer(remainParsed.data(), remainParsed.size()), ec);
  remainParsed.erase(remainParsed.begin(), remainParsed.begin() + consumed);
  if (requestParser.is_done()) {
    return createRequest(requestParser.release());
  }
  if (ec == http::error::need_more || ec.value() == 0) {
    ec.clear();
  } else {
    std::string errmsg = "400|" + ec.message();
    throw std::invalid_argument(errmsg);
  }
  return NULL;
}

Response *
HttpParser::createResponse(http::response<http::string_body> message) {
  std::string status_code = std::to_string(message.result_int());
  unsigned int version = message.version();
  std::string reason = message.reason().to_string();

  std::map<std::string, std::string> responseHeader;
  collectHeaderKV<false>(message, responseHeader);

  Response *response = new Response(version, status_code, reason,
                                    responseHeader, message.body());

  return response;
}

Response *HttpParser::parseResponse(const char *msg, const size_t len) {
  addToRemainParsed(msg, len);
  boost::system::error_code ec;
  size_t consumed = responseParser.put(
      boost::asio::buffer(remainParsed.data(), remainParsed.size()), ec);
  remainParsed.erase(remainParsed.begin(), remainParsed.begin() + consumed);
  if (responseParser.is_done()) {
    return createResponse(responseParser.release());
  }
  if (ec == http::error::need_more || ec.value() == 0) {
    ec.clear();
  } else {
    std::string errmsg = "400|"+ec.message();
    throw std::invalid_argument(errmsg);
  }
  return NULL;
}
