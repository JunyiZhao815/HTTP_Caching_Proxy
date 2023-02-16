#include "HttpParser.h"

request_line_t HttpParser::parseRequestLine(std::string &line) {
  std::vector<std::string> args;
  boost::split(args, line, boost::is_any_of(" "), boost::token_compress_on);
  request_line_t request_line;
  request_line.method = args.at(0);
  request_line.uri = args.at(1);
  request_line.version = args.at(2);
  return request_line;
}

status_line_t HttpParser::parseStatusLine(std::string &line) {
  std::vector<std::string> args;
  boost::split(args, line, boost::is_any_of(" "));
  status_line_t status_line;
  status_line.version = args.at(0);
  status_line.status_code = args.at(1);
  status_line.reason = args.at(2);
  return status_line;
}

HttpParser::http_lines_t HttpParser::organizeLines(std::string &message) {
  http_lines_t http_lines;
  std::vector<std::string> lines;
  boost::split(lines, message, boost::is_any_of("\n"));
  http_lines.startLine = lines[0];
  // get header lines
  size_t m;
  for (m = 1; m < lines.size(); m++) {
    if (lines[m].length() == 1 && lines[m] == "\r") {
      // pass empty line splitting header and body
      m += 1;
      break;
    }
    lines[m].erase(lines[m].end() - 1); // remove last \r
    http_lines.header.insert(line2Pair(lines[m]));
  }
  // get body
  for (; m < lines.size(); m++) {
    http_lines.body.append(lines[m]).append("\n");
  }
  http_lines.body.erase(http_lines.body.end() - 1); // remove last \n
  return http_lines;
}

std::pair<std::string, std::string> HttpParser::line2Pair(std::string &line) {
  size_t pos = line.find_first_of(":");
  std::pair<std::string, std::string> pair;
  pair.first = line.substr(0, pos);
  pair.second = line.substr(pos + 1);
  boost::trim_left(pair.second); // key:  value
  return pair;
}

void HttpParser::print_request(request_t &request) {
  std::cout << "[Request]" << std::endl;
  std::cout << "[Start Line]" << std::endl;
  std::cout << request.request_line.method << "; " << request.request_line.uri
            << "; " << request.request_line.version << std::endl;

  std::cout << "[Header Lines]" << std::endl;
  std::map<std::string, std::string>::iterator iter;
  for (iter = request.header.begin(); iter != request.header.end(); ++iter) {
    std::cout << "key:" << iter->first << "; value:" << iter->second
              << std::endl;
  }
  std::cout << "[Body]" << std::endl;
  std::cout << request.body << std::endl;
}

void HttpParser::print_response(response_t &response) {
  std::cout << "[Response]" << std::endl;
  std::cout << "[Start Line]" << std::endl;
  std::cout << response.status_line.version << "; "
            << response.status_line.status_code << "; "
            << response.status_line.reason << std::endl;

  std::cout << "[Header Lines]" << std::endl;
  std::map<std::string, std::string>::iterator iter;
  for (iter = response.header.begin(); iter != response.header.end(); ++iter) {
    std::cout << "key:" << iter->first << "; value:" << iter->second
              << std::endl;
  }
  std::cout << "[Body]" << std::endl;
  std::cout << response.body << std::endl;
}

request_t HttpParser::parseRequest(std::string &message) {
  http_lines_t http_lines = organizeLines(message);
  request_t request;
  request.header = http_lines.header;
  request.body = http_lines.body;
  request.request_line = parseRequestLine(http_lines.startLine);
  return request;
}

response_t HttpParser::parseResponse(std::string &message) {
  http_lines_t http_lines = organizeLines(message);
  response_t response;
  response.header = http_lines.header;
  response.body = http_lines.body;
  response.status_line = parseStatusLine(http_lines.startLine);
  return response;
}

std::pair<std::string, std::string>
HttpParser::separateIpPort(std::string &ipaddr, std::string &defaultPort) {
  std::pair<std::string, std::string> ip_port;
  size_t pos = ipaddr.find_first_of(":");
  if (pos == std::string::npos) {
    // only ip, not port
    ip_port.first = ipaddr;
    ip_port.second = defaultPort;
  } else {
    // ip:port
    ip_port.first = ipaddr.substr(0, pos);
    ip_port.second = ipaddr.substr(pos + 1);
  }

  return ip_port;
}
