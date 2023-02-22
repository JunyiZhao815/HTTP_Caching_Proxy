#include "HttpConnector.h"
#include <string_view>

HttpConnector::HttpConnector(const int c_fd, const int client_id)
    : client_socket_fd(c_fd), server_socket_fd(-1), client_id(client_id) {
#ifdef DEBUG
  std::cout << "new connector:" << client_id << std::endl;
#endif
}

HttpConnector::~HttpConnector() {
#ifdef DEBUG
  std::cout << "[" << getClientId() << "] finish method\n";
#endif
}

void HttpConnector::set_server_socket_fd(int s_fd) { server_socket_fd = s_fd; }

Request *HttpConnector::receiveRequest() {
  HttpParser httpParser;
  size_t msglen;
  Request *request = NULL;
  while (request == NULL) {
    void *msg = tcpConnector.receiveMessage(client_socket_fd, msglen);
    request = httpParser.parseRequest((char *)msg, msglen);
    // package is not complete but client close connection
    if(msglen == 0 && request == NULL){
      free(msg);
      throw std::runtime_error("Client close connection");
    }
    free(msg);
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
    if(msglen == 0 && response == NULL){
      free(msg);
      throw std::runtime_error("Server close connection");
    }
    free(msg);
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
  struct timeval tv;
  tv.tv_sec = 10;
  tv.tv_usec = 0;
  while (true) {
    FD_ZERO(&readfds);
    FD_SET(client_socket_fd, &readfds);
    FD_SET(server_socket_fd, &readfds);
    int event_nums = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
    if (event_nums > 0) {
      if (FD_ISSET(client_socket_fd, &readfds)) {
        // client -> server
        client_len = transportRawMessage(client_socket_fd, server_socket_fd);
      }
      if (FD_ISSET(server_socket_fd, &readfds)) {
        // server -> client
        server_len = transportRawMessage(server_socket_fd, client_socket_fd);
      }
      // one ternal close the connection
      if (client_len == 0 || server_len == 0) {
        return;
      }
    } else if(event_nums == 0){
      throw std::runtime_error("Tunnel closed between client and server due to timeout");
    }else{
      throw std::runtime_error(
          "Tunnel closed between client and server by accident");
    }
  }
}

void HttpConnector::sendMessage(HttpMessage &httpMessage, bool isSendToClient) {
  std::string ss = httpMessage.message2string();
  if (isSendToClient) {
    tcpConnector.sendMessage(client_socket_fd, ss.c_str(), ss.length());
  } else {
    tcpConnector.sendMessage(server_socket_fd, ss.c_str(), ss.length());
  }
}

size_t HttpConnector::getClientId() { return client_id; }
