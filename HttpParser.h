#include "boost/algorithm/string.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

typedef struct {
  std::string method;
  std::string uri;
  std::string version;
} request_line_t;

typedef struct {
  std::string version;
  std::string status_code;
  std::string reason;
} status_line_t;

typedef struct {
  request_line_t request_line;
  std::map<std::string, std::string> header;
  std::string body;
} request_t;

typedef struct {
  status_line_t status_line;
  std::map<std::string, std::string> header;
  std::string body;
} response_t;

class HttpMethod {};

class HttpGet : public HttpMethod {};

class HttpPost : public HttpMethod {};

class HttpConnect : public HttpMethod {};

class HttpParser {
private:
  typedef struct {
    std::string startLine;
    std::map<std::string, std::string> header;
    std::string body;
  } http_lines_t;

private:
  /*
   * Parse string into request line struct
   */
  request_line_t parseRequestLine(std::string &line);
  /*
   * Parse string into status line struct
   */
  status_line_t parseStatusLine(std::string &line);
  /*
   * Organize http message into http_lines, containing first line, head, and
   * body
   */
  http_lines_t organizeLines(std::string &message);
  /*
   * Parse string into key value pair
   * line in the format of key:value or key:  value
   */
  std::pair<std::string, std::string> line2Pair(std::string &line);

public:
  /*
   * Parse string into request
   */
  request_t parseRequest(std::string &message);

  /*
   * Parse string into response
   */
  response_t parseResponse(std::string &message);

  // for test
  void print_request(request_t &request);

  void print_response(response_t &response);

  /*
   * Separate ip:port to <ip, port>
   */
  std::pair<std::string, std::string> separateIpPort(std::string &ipaddr,
                                                     std::string &defaultPort);
};
