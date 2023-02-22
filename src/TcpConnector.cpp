#include "TcpConnector.h"

void TcpConnector::commonError(const std::string &description) {
  std::string errmsg = "Error: " + description;
  throw std::runtime_error(errmsg);
}

void TcpConnector::error(const std::string &hostname, const std::string &port,
                         const std::string &description) {
  std::string errmsg =
      "Error: " + description + " (" + hostname + ", " + port + ")";
  throw std::runtime_error(errmsg);
}

int TcpConnector::initializeServerSocket(const char *port) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    error(hostname, port, "cannot get address info for host");
  }
  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    error(hostname, port, "cannot create socket");
  }
  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    error(hostname, port, "cannot bind socket");
  }
  status = listen(socket_fd, BACKLOG);
  if (status == -1) {
    error(hostname, port, "cannot listen on socket");
  }
  freeaddrinfo(host_info_list);
  return socket_fd;
}

int TcpConnector::initializeClientSocket(const char *hostname,
                                         const char *port) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    error(hostname, port, "cannot get address info for host");
  }
  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    error(hostname, port, "cannot create socket");
  }
  status =
      connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    error(hostname, port, "cannot connect to socket");
  }
  freeaddrinfo(host_info_list);
  return socket_fd;
}

int TcpConnector::waitAcceptConnect(int socket_fd) {
  struct sockaddr_storage addr;
  socklen_t socket_addr_len = sizeof(addr);
  int client_connection_fd =
      accept(socket_fd, (struct sockaddr *)&addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    commonError("cannot accept connection on socket");
  }
  return client_connection_fd;
}

void *TcpConnector::receiveMessage(const int client_connection_fd,
                                   size_t &len) {
  void *buffer = malloc(BUFFER_SIZE);
  if (buffer == NULL) {
    commonError("Allocate memory for received message failed");
  }
  memset(buffer, 0, BUFFER_SIZE);
  int ret = recv(client_connection_fd, buffer, BUFFER_SIZE, 0);
  if(ret == -1){
    commonError("Receive Message failed due to timeout");
  }else{
    len = ret;
  }

  return buffer;
}

void TcpConnector::sendMessage(const int sockfd, const void *msg,
                               const size_t len) {
  int status = send(sockfd, msg, len, 0);
  if (status == -1) {
    commonError("send message fail");
  }
}
