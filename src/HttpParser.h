#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__
#include "Request.h"
#include "Response.h"
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;

class HttpParser {
private:
  std::vector<char> remainParsed;
  /*
   * Requst parser helper
   */
  http::parser<true, http::string_body> requestParser;

  /*
   * Response parser helper
   */
  http::parser<false, http::string_body> responseParser;

private:
  /*
   * Add msg not been parsed in to remainParsed
   * @msg remained msg
   * @len length of msg
   */
  void addToRemainParsed(const char *msg, const size_t len);
  /*
   * Collect header key-value from parsed message
   * @message request/response parsed message
   * @header result container
   */
  template <bool isrequest>
  void collectHeaderKV(http::message<isrequest, http::string_body> &message,
                       std::map<std::string, std::string> &header) {
    std::stringstream ss;
    http::request<http::string_body>::iterator iter;
    for (iter = message.begin(); iter != message.end(); ++iter) {
      std::pair<std::string, std::string> name_value;
      ss << iter->name_string();
      name_value.first = ss.str();
      ss.str("");
      ss << iter->value();
      name_value.second = ss.str();
      ss.str("");
      header.insert(name_value);
    }
  }
  /*
   * Create request by request message
   * @message parsed message
   * @return parsed request
   */
  Request *createRequest(http::request<http::string_body> message);

  /*
   * Create response by response message
   * @message parsed message
   * @return parsed response
   */
  Response *createResponse(http::response<http::string_body> message);

public:
  /*
   * Parse msg with length of len to request
   */
  Request *parseRequest(const char *msg, const size_t len);

  /*
   * Parse msg with length of len to response
   */
  Response *parseResponse(const char *msg, const size_t len);
};

#endif
