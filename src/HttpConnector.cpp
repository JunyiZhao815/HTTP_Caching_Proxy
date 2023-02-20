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

size_t HttpConnector::transportRawMessage(const int from_socket,
                                          const int to_socket) {
  size_t len;
  void *msg = tcpConnector.receiveMessage(from_socket, len);
  if (len != 0) {
    tcpConnector.sendMessage(to_socket, msg, len);
  }
  free(msg);
  return len;
}

void HttpConnector::tunnelTransport() {
  fd_set readfds;
  int client_len = -1;
  int server_len = -1;
  while (true) {
    FD_ZERO(&readfds);
    FD_SET(client_socket_fd, &readfds);
    FD_SET(server_socket_fd, &readfds);
    int event_nums = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
    if (event_nums >= 0) {
      if (FD_ISSET(client_socket_fd, &readfds)) {
        // client -> server
        client_len = transportRawMessage(client_socket_fd, server_socket_fd);
        std::cout << "c->s, " << client_len << std::endl;
      }
      if (FD_ISSET(server_socket_fd, &readfds)) {
        // server -> client
        server_len = transportRawMessage(server_socket_fd, client_socket_fd);
        std::cout << "s->c, " << server_len << std::endl;
      }
      // one ternal close the connection
      if (client_len == 0 || server_len == 0) {
        return;
      }
    } else {
      throw std::runtime_error(
          "tunnel closed by accident between client and server");
    }
  }
}
