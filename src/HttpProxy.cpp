#include "HttpProxy.h"

void HttpProxy::filtHttpMethod(const std::string method) {
  if (!(method == "GET" || method == "POST" || method == "CONNECT")) {
    throw std::invalid_argument("405");
  }
}

void HttpProxy::methodAct(const std::string &method,
                          HttpConnector &httpConnector, Request &request) {
  HttpMethod *httpMethod;
  std::cout<<method<<std::endl;
  if (method == "GET") {
    httpMethod = new GetMethod();
  } else if (method == "POST") {
    httpMethod = new PostMethod();
  } else {
    httpMethod = new ConnectMethod();
  }
  httpMethod->takeAction(httpConnector, request);
  delete httpMethod;
}

void HttpProxy::_exec(int client_socket_fd) {
  HttpConnector httpConnector(client_socket_fd);
  try {
    // receive client request
    Request *request = httpConnector.receiveRequest();

    filtHttpMethod(request->getMethod());
    std::pair<std::string, std::string> ip_port = request->getHost();
    // connect with server
    int server_socket_fd = tcpConnector.initializeClientSocket(
        ip_port.first.c_str(), ip_port.second.c_str());
    httpConnector.set_server_socket_fd(server_socket_fd);
    // execute method
    methodAct(request->getMethod(), httpConnector, *request);
    // close server fd and free memory
    delete request;
    close(server_socket_fd);
  } catch (std::invalid_argument& e) {
    sendErrorStatus(e.what(), "Client Agent Error", httpConnector);
  }
}

void HttpProxy::exec(int client_socket_fd) {
  try {
    _exec(client_socket_fd);
  } catch (std::runtime_error& e) {
    // TODO transform to log
    std::cout << e.what() << std::endl;
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

void HttpProxy::init(std::string port) {
  proxy_fd = tcpConnector.initializeServerSocket(port.c_str());
}

void HttpProxy::sendErrorStatus(std::string status_code, std::string reason,
                                HttpConnector &httpConnector) {
  ResponseFactory factory;
  Response response = factory.create_4xx_response(status_code, reason);
  httpConnector.sendResponse(&response);
}

void HttpProxy::noneMultiThread() {
  int client_socket_fd = tcpConnector.waitAcceptConnect(proxy_fd);
  exec(client_socket_fd);
  close(client_socket_fd);
}
