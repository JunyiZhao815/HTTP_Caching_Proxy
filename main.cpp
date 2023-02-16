#include "HttpParser.h"
#include "TcpConnector.h"

int main() {
  // receive client request
  TcpConnector tcp;
  int socket_fd = tcp.initializeServerSocket("4000");
  int client_fd = tcp.waitAcceptConnect(socket_fd);
  void *msg = tcp.receiveMessage(client_fd);

  HttpParser parser;
  std::string smsg = (char *)msg;
  request_t request = parser.parseRequest(smsg);
  parser.print_request(request);

  // request target server
  std::string defaultPort = "80";
  std::pair<std::string, std::string> ip_port =
      parser.separateIpPort(request.header["Host"], defaultPort);
  int target_fd =
      tcp.initializeClientSocket(ip_port.first.c_str(), ip_port.second.c_str());
  tcp.sendMessage(target_fd, msg, BUFFER_SIZE);

  // receive message from server
  msg = tcp.receiveMessage(target_fd);
  smsg = (char *)msg;
  response_t response = parser.parseResponse(smsg);
  parser.print_response(response);

  // send back to client
  tcp.sendMessage(client_fd, msg, BUFFER_SIZE);
}
