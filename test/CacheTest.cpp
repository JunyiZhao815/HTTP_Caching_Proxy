#include "CacheTest.h"

void CacheTest::onRun() {
    test_cache_map();
}


void CacheTest::test_cache_map(std::string port)
 {
  TcpConnector tcpConnector;
  HttpParser httpParser;
  // wait client connect
  int fd = tcpConnector.initializeServerSocket(port.c_str());
  int c_fd = tcpConnector.waitAcceptConnect(fd);
  HttpConnector httpConnector(c_fd, 1);
  // receive client request
  Request *request = httpConnector.receiveRequest();
  // get request date
  time_t request_t = time(NULL);
  std::cout << "current time is: " << request_t << std::endl;
  //
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
  //  send back response to client
  httpConnector.sendResponse(response);
  // Cache define
  Cache cache(httpConnector, 3, 0);
  cache.putResponse(*request, *response);
  std::map<std::string, Node*>::iterator it = cache.map.begin();
  while(it != cache.map.end()){
    std::cout<<"key: " << it->first << ", value: " << (it->second)->response.getDate() << std::endl;
    it++;
  }

// test isFresh
  std::cout << "The request is fresh or not? " << cache.isFresh(*request, 0, request_t) << std::endl;
// test validation
  
  cache.revalidation(0,  *response, *request);
  
  delete request;
  delete response;
  close(s_fd);
  close(c_fd);
  close(fd);
}
