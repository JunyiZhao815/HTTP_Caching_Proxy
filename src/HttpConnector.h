#ifndef __HTTP_CONNECTOR_H__
#define __HTTP_CONNECTOR_H__
#include "HttpParser.h"
#include "TcpConnector.h"

class HttpConnector {
private:
  int client_socket_fd;
  int server_socket_fd;
  TcpConnector tcpConnector;

private:
  /*
   * Transport message without any parse, send forward directly
   * @from_socket where to send
   * @to_socket send to where
   * @return length of message transported
   */
  size_t transportRawMessage(const int from_socket, const int to_socket);

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

  /*
   * Create tunnel between user and server,
   * keep connection until one close socket
   */
  void tunnelTransport();

  /*
   * Send request/response to receiver
   * @httpMessage request/response
   * @receiver_fd receiver socket fd
   * @isSendToClient true:send to client, false: send to server
   */
  void sendMessage(HttpMessage &httpMessage, bool isSendToClient);
};

#endif
