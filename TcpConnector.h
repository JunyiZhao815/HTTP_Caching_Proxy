#include <arpa/inet.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#define BACKLOG 100
#define BUFFER_SIZE 10240

class TcpConnector {
private:
  void commonError(const char *description);

  void error(const char *hostname, const char *port, const char *description);

public:
  /*
   * Initialize tcp socket server
   * @port listen port
   * @return socket_fd
   */
  int initializeServerSocket(const char *port);

  /*
   * Initialize tcp socket client
   * @port server port
   * @hostname server hostname
   * @return socket_fd
   */
  int initializeClientSocket(const char *hostname, const char *port);

  /*
   * Receive message from client
   * @client_connection_fd target client
   * @return message
   */
  void *receiveMessage(int client_connection_fd);

  /*
   * Accept client connection
   * @socket_fd server socket_fd
   * @return client socket_fd
   */
  int waitAcceptConnect(int socket_fd);

  /*
   * Send message to client
   * @sockfd target client
   * @msg message to send
   * @len length of msg
   */
  void sendMessage(int sockfd, const void *msg, size_t len);
};
