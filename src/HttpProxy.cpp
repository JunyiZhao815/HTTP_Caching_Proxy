#include "HttpProxy.h"

size_t HttpProxy::next_client_id = 1;

HttpProxy::HttpProxy(Cache &cache) : cache(cache) {}

size_t HttpProxy::assignClientId() {
  size_t id;
  id_mutex.lock();
  id = next_client_id;
  next_client_id += 1;
  id_mutex.unlock();
  return id;
}

void HttpProxy::methodAct(const std::string &method,
                          HttpConnector &httpConnector, Request &request) {
  HttpMethod *httpMethod;
#ifdef DEBUG
  std::cout << "[" << httpConnector.getClientId() << "] " << method
            << std::endl;
#endif
  if (method == "GET") {
    httpMethod = new GetMethod();
  } else if (method == "POST") {
    httpMethod = new PostMethod();
  } else if (method == "CONNECT") {
    httpMethod = new ConnectMethod();
  } else {
#ifdef DEBUG
    std::cout << "Method not support, cid:" << httpConnector.getClientId()
              << std::endl;
#endif
    throw std::invalid_argument("405|ERROR Method not support");
  }
  std::unique_ptr<HttpMethod> method_ptr(httpMethod);
  method_ptr->takeAction(httpConnector, request, cache);
}

void HttpProxy::parseInvalidArgExp(const std::string &exception,
                                   std::string &status_code,
                                   std::string &reason) {
  size_t index = exception.find('|');
#ifdef DEBUG
  assert(index != std::string::npos);
#endif
  status_code = exception.substr(0, index);
  reason = exception.substr(index + 1);
}

void HttpProxy::_exec(int client_socket_fd, int client_id) {
  HttpConnector httpConnector(client_socket_fd, client_id);
  try {
    // receive client request
    Request *request = httpConnector.receiveRequest();
    logNewRequest(client_socket_fd, *request,
                  client_id); // log new request information
    std::unique_ptr<Request> req_ptr(request);
    std::pair<std::string, std::string> ip_port = req_ptr->getHost();
    // connect with server
    int server_socket_fd = tcpConnector.initializeClientSocket(
        ip_port.first.c_str(), ip_port.second.c_str());
    httpConnector.set_server_socket_fd(server_socket_fd);
    // execute method
    methodAct(req_ptr->getMethod(), httpConnector, *req_ptr);
    // close server fd and free memory
    close(server_socket_fd);
  } catch (std::invalid_argument &e) {
    std::string status_code;
    std::string reason;
    parseInvalidArgExp(e.what(), status_code, reason);
    sendErrorStatus(status_code, reason, httpConnector);
    //Logger::getLogger().proxyLog(client_id, reason);
  }
}

void HttpProxy::exec(int client_socket_fd) {
  size_t client_id = assignClientId();
  try {
    _exec(client_socket_fd, client_id);
  } catch (std::runtime_error &e) {
    Logger::getLogger().proxyLog(client_id, e.what());
  } catch (std::exception &e) {
    Logger::getLogger().proxyLog(client_id, e.what());
  }
  close(client_socket_fd);
}

void HttpProxy::init(std::string port) {
  proxy_fd = tcpConnector.initializeServerSocket(port.c_str());
}

void HttpProxy::sendErrorStatus(const std::string &status_code,
                                const std::string &reason,
                                HttpConnector &httpConnector) {
  ResponseFactory factory;
  Response response = factory.create_4xx_response(status_code, reason);
  httpConnector.sendResponse(&response);
  Logger::getLogger().proxyLog(httpConnector.getClientId(), "Responding \"" + response.getFirstLine() + "\"");
}

void HttpProxy::noneMultiThread() {
  int client_socket_fd = tcpConnector.waitAcceptConnect(proxy_fd);
  exec(client_socket_fd);
  // close(client_socket_fd);
}

void HttpProxy::multiThread() {
  while (true) {
    try {
      int client_socket_fd = tcpConnector.waitAcceptConnect(proxy_fd);
      struct timeval timeout;
      timeout.tv_sec = 10;
      timeout.tv_usec = 0;
      setClientSocketFdRecvTimeout(client_socket_fd,
                                   timeout); // set recv only wait specify secs
      std::thread t(&HttpProxy::exec, this, client_socket_fd);
      t.detach();
    } catch (std::runtime_error &e) {
      Logger::getLogger().log(e.what());
    } catch (std::exception &e) {
      Logger::getLogger().log(e.what());
    }
  }
}

void HttpProxy::setClientSocketFdRecvTimeout(int client_socket_fd,
                                             struct timeval timeout) {
  setsockopt(client_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
             sizeof(timeout));
}

void HttpProxy::logNewRequest(int client_socket_fd, Request &request, int id) {
  TcpConnector tcpConnector;
  std::string requestIp = tcpConnector.getIpByFd(client_socket_fd);
  std::string logInfo =
      "\"" + request.getFirstLine() + "\" from " + requestIp + " @ ";
  std::string currTime = getCurrUTCtime();
  logInfo += currTime;
  Logger::getLogger().proxyLog(id, logInfo);
}

std::string HttpProxy::getCurrUTCtime() {
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t utc_time = std::chrono::system_clock::to_time_t(now);
  std::tm utc_tm = *std::gmtime(&utc_time);
  std::stringstream ss;
  ss << std::put_time(&utc_tm, "%a %b %d %T %Y");
  return ss.str();
}
