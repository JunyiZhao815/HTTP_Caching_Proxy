#include "HttpConnectorTest.h"
#include "../src/TcpConnector.h"
#include <cstdlib>

void HttpConnectorTest::onRun() { test_simple_proxy(); }

void HttpConnectorTest::test_simple_proxy(std::string port) {
  TcpConnector tcpConnector;
  HttpParser httpParser;
  // wait client connect
  int fd = tcpConnector.initializeServerSocket(port.c_str());
  int c_fd = tcpConnector.waitAcceptConnect(fd);
  HttpConnector httpConnector(c_fd);
  // receive client request
  Request *request = httpConnector.receiveRequest();
  std::cout << "client request:\n";
  std::cout << *request << std::endl;
  std::pair<std::string, std::string> ip_port = request->getHost();
  // connect to server
  int s_fd = tcpConnector.initializeClientSocket(ip_port.first.c_str(),
                                                 ip_port.second.c_str());
  httpConnector.set_server_socket_fd(s_fd);
  std::cout << "connect to server (" << ip_port.first << ", " << ip_port.second
            << ") successfully\n";
  // send request to server
  httpConnector.sendRequest(request);
  // receive server response
  Response *response = httpConnector.receiveResponse();
  std::cout << "server response:\n";
  std::cout << *response << std::endl;
  // send back response to client
  httpConnector.sendResponse(response);
}
