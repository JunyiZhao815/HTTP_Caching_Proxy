#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__
#include "Request.h"
#include "Response.h"
#include <boost/algorithm/string.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <vector>
#include <boost/optional/optional_io.hpp>

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

  bool checkHeaderFiledValid(std::pair<std::string, std::string>& name_value);

  void checkDateValid(std::string& date);

  /*
   * Add msg not been parsed in to remainParsed
   * @msg remained msg
   * @len length of msg
   */
  void addToRemainParsed(const char *msg, const size_t len);
  /*
   * Collect header key-value from parsed message
   * note key would be all transform to lower
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
      std::string name = ss.str();
      boost::algorithm::to_lower(name); // tranform name to lower
      name_value.first = name;
      ss.str("");
      ss << iter->value();
      name_value.second = ss.str();
      ss.str("");
      if(checkHeaderFiledValid(name_value)){
        header.insert(name_value);
      }
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

  /*
   * Handle chunked response, delete chunked, trailer from header, reset content-length
   */
  void handleChunked(Response* response);

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
