#include "GetMethodTest.h"

void GetMethodTest::onRun() {
  // test_simple_get();
  test_get_by_while();
}

void GetMethodTest::test_simple_get(std::string port) {
  Cache cache(100, 0);
  TcpConnector tcpConnector;
  HttpParser httpParser;
  // wait client connect
  int fd = tcpConnector.initializeServerSocket(port.c_str());
  int c_fd = tcpConnector.waitAcceptConnect(fd);
  HttpConnector httpConnector(c_fd, 1);
  // receive client request
  Request *request = httpConnector.receiveRequest();
  std::cout << "client request:\n";
  std::cout << *request << std::endl;
  std::pair<std::string, std::string> ip_port = request->getHost();
  // assert get method
  assert(ip_port.second == "80" && request->getMethod() == "GET");
  // connect to server
  int s_fd = tcpConnector.initializeClientSocket(ip_port.first.c_str(),
                                                 ip_port.second.c_str());
  httpConnector.set_server_socket_fd(s_fd);
  std::cout << "connect to server (" << ip_port.first << ", " << ip_port.second
            << ") successfully\n";
  GetMethod getMethod;
  getMethod.takeAction(httpConnector, *request, cache);

  close(c_fd);
  close(s_fd);
  close(fd);
  delete request;
}

void GetMethodTest::test_get_by_while(std::string port) {
  Cache cache(100, 0);
  TcpConnector tcpConnector;
  HttpParser httpParser;
  // wait client connect
  int fd = tcpConnector.initializeServerSocket(port.c_str());
  while (true) {
    int c_fd = tcpConnector.waitAcceptConnect(fd);
    HttpConnector httpConnector(c_fd, 1);
    // receive client request
    Request *request = httpConnector.receiveRequest();
    std::cout << "client request:\n";
    std::cout << *request << std::endl;
    std::pair<std::string, std::string> ip_port = request->getHost();
    // assert get method
    assert(ip_port.second == "80" && request->getMethod() == "GET");
    // connect to server
    int s_fd = tcpConnector.initializeClientSocket(ip_port.first.c_str(),
                                                   ip_port.second.c_str());
    httpConnector.set_server_socket_fd(s_fd);
    std::cout << "connect to server (" << ip_port.first << ", "
              << ip_port.second << ") successfully\n";
    GetMethod getMethod;
    getMethod.takeAction(httpConnector, *request, cache);

    close(c_fd);
    close(s_fd);
    delete request;
  }
  close(fd);
}
