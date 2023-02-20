#include "HttpConnector.h"
#include <string_view>

HttpConnector::HttpConnector(const int c_fd)
    : client_socket_fd(c_fd), server_socket_fd(-1) {}

void HttpConnector::set_server_socket_fd(int s_fd) { server_socket_fd = s_fd; }

Request *HttpConnector::receiveRequest() {
  HttpParser httpParser;
  size_t msglen;
  Request *request = NULL;
  while (request == NULL) {
    void *msg = tcpConnector.receiveMessage(client_socket_fd, msglen);
    request = httpParser.parseRequest((char *)msg, msglen);
  }
  return request;
}

Response *HttpConnector::receiveResponse() {
  HttpParser httpParser;
  size_t msglen;
  Response *response = NULL;
  while (response == NULL) {
    void *msg = tcpConnector.receiveMessage(server_socket_fd, msglen);
    response = httpParser.parseResponse((char *)msg, msglen);
  }
  return response;
}

void HttpConnector::sendRequest(const Request *request) {
  std::stringstream ss;
  ss << *request;
  tcpConnector.sendMessage(server_socket_fd, ss.str().c_str(),
                           ss.str().length());
}

void HttpConnector::sendResponse(const Response *response) {
  std::stringstream ss;
  ss << *response;
  tcpConnector.sendMessage(client_socket_fd, ss.str().c_str(),
                           ss.str().length());
}
