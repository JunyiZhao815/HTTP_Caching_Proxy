#include "PostMethodTest.h"

void PostMethodTest::onRun() { test_simple_post(); }

void PostMethodTest::test_simple_post(std::string port) {
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
  // assert get method
  assert(ip_port.second == "80" && request->getMethod() == "POST");
  // connect to server
  int s_fd = tcpConnector.initializeClientSocket(ip_port.first.c_str(),
                                                 ip_port.second.c_str());
  httpConnector.set_server_socket_fd(s_fd);
  std::cout << "connect to server (" << ip_port.first << ", " << ip_port.second
            << ") successfully\n";
  PostMethod postMethod;
  postMethod.takeAction(httpConnector, *request);

  close(c_fd);
  close(s_fd);
  close(fd);
  delete request;
}