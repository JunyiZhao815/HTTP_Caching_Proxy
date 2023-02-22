#ifndef __TCP_CONNECTOR_H__
#define __TCP_CONNECTOR_H__

#include <arpa/inet.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#define BACKLOG 100
#define BUFFER_SIZE 10240

class TcpConnector {
private:
  void commonError(const std::string &description);

  void error(const std::string &hostname, const std::string &port,
             const std::string &description);

public:
  /*
   * Initialize tcp socket server
   * @port listen port
   * @return socket_fd
   */
  int initializeServerSocket(const char *port);

  /*
   * Initialize tcp socket client
   * @hostname server hostname
   * @port server port
   * @return socket_fd
   */
  int initializeClientSocket(const char *hostname, const char *port);

  /*
   * Receive message from client
   * @client_connection_fd target client
   * @len received message length
   * @return message
   */
  void *receiveMessage(const int client_connection_fd, size_t &len);

  /*
   * Accept client connection
   * @socket_fd server socket_fd
   * @return client socket_fd
   */
  int waitAcceptConnect(const int socket_fd);

  /*
   * Send message to client
   * @sockfd target client
   * @msg message to send
   * @len length of msg
   */
  void sendMessage(const int sockfd, const void *msg, const size_t len);
};

#endif
