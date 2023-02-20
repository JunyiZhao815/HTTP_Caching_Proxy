#include "ConnectMethodTest.h"

void ConnectMethodTest::onRun() { test_simple_proxy(); }

void ConnectMethodTest::test_simple_proxy(std::string port) {
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
  // assert connect method
  assert(ip_port.second == "443");
  // connect to server
  int s_fd = tcpConnector.initializeClientSocket(ip_port.first.c_str(),
                                                 ip_port.second.c_str());
  httpConnector.set_server_socket_fd(s_fd);
  std::cout << "connect to server (" << ip_port.first << ", " << ip_port.second
            << ") successfully\n";
  // make connect method take action
  ConnectMethod connectMethod;
  connectMethod.takeAction(httpConnector);

  close(c_fd);
  close(s_fd);
  close(fd);
  delete request;
}
