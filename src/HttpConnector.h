#ifndef __HTTP_CONNECTOR_H__
#define __HTTP_CONNECTOR_H__
#include "HttpParser.h"
#include "TcpConnector.h"

class HttpConnector {
private:
  int client_socket_fd;
  int server_socket_fd;
  TcpConnector tcpConnector;

public:
  HttpConnector(const int c_fd);
  /*
   * Set server_socket_fd
   */
  void set_server_socket_fd(int s_fd);
  /*
   * Receive request from client agent
   */
  Request *receiveRequest();

  /*
   * Receive response from origin server
   */
  Response *receiveResponse();

  /*
   * Send forward request to origin server
   */
  void sendRequest(const Request *request);

  /*
   * Send response back to client agent
   */
  void sendResponse(const Response *response);
};

#endif
