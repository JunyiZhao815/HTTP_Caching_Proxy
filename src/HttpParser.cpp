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
  size_t consumed = 1;
  while (consumed != 0 && !requestParser.is_done() && !remainParsed.empty()) {
    consumed = requestParser.put(
        boost::asio::buffer(remainParsed.data(), remainParsed.size()), ec);
    remainParsed.erase(remainParsed.begin(), remainParsed.begin() + consumed);
  }

  if (requestParser.is_done()) {
    if (!remainParsed.empty()) {
      // content-length is wrong
      throw std::invalid_argument(
          "400|ERROR Content-Length is not match with the body size");
    }
    return createRequest(requestParser.release());
  }
  if (ec == http::error::need_more || ec.value() == 0) {
    ec.clear();
  } else {
    std::string errmsg = "400|ERROR " + ec.message();
    throw std::invalid_argument(errmsg);
  }
  return NULL;
}

void HttpParser::handleChunked(Response *response) {
  // delete chunked from Transfer-Encoding
  std::string transferEncoding = response->getTransferEncoding();
  if (transferEncoding == "chunked") {
    response->removeHeaderField("transfer-encoding");
  } else {
    size_t index = transferEncoding.find(", chunked");
    transferEncoding = transferEncoding.substr(0, index);
    response->addHeaderField("transfer-encoding", transferEncoding);
  }
  // remove trailer-part
  response->removeHeaderField("trailer-part");
  // add Content-Length
  size_t contentLength = response->getContentLen();
  response->addHeaderField("content-length", std::to_string(contentLength));
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
  if (message.chunked()) {
    handleChunked(response);
  }
  return response;
}

Response *HttpParser::parseResponse(const char *msg, const size_t len) {
  addToRemainParsed(msg, len);
  boost::system::error_code ec;
  size_t consumed = 1;
  while (consumed != 0 && !responseParser.is_done() && !remainParsed.empty()) {
    consumed = responseParser.put(
        boost::asio::buffer(remainParsed.data(), remainParsed.size()), ec);
    remainParsed.erase(remainParsed.begin(), remainParsed.begin() + consumed);
  }
  if (responseParser.is_done()) {
    if (!remainParsed.empty()) {
      throw std::invalid_argument(
          "500|ERROR The Content-Length of response from origin server is not "
          "match with body size");
    }
    return createResponse(responseParser.release());
  }
  if (ec == http::error::need_more || ec.value() == 0) {
    ec.clear();
  } else {
    std::string errmsg = "502|ERROR " + ec.message();
    throw std::invalid_argument(errmsg);
  }
  return NULL;
}

bool HttpParser::checkHeaderFiledValid(
    std::pair<std::string, std::string> &name_value) {
  std::string name = name_value.first;
  std::string value = name_value.second;
  if (name == "date" || name == "last-modified") {
    checkDateValid(value);
  }else if(name == "expires"){
    try{
      checkDateValid(value);
    }catch(std::invalid_argument& e){
      return false;
    }
  }
  return true;
}

void HttpParser::checkDateValid(std::string &date) {
  size_t gmt = date.find("GMT");
  if (gmt != std::string::npos) {
    date.erase(gmt - 1, 4);
  } else {
    throw std::invalid_argument(
        "500|ERROR Invalid date format received from origin server");
  }
  struct tm tm;
  if (strptime(date.c_str(), "%a, %d %b %Y %H:%M:%S", &tm) == NULL) {
    throw std::invalid_argument(
        "500|ERROR Invalid date format received from origin server");
  }
}
