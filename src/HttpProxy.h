#ifndef __HTTP_PROXY_H__
#define __HTTP_PROXY_H__
#include "ConnectMethod.h"
#include "GetMethod.h"
#include "HttpConnector.h"
#include "Logger.h"
#include "PostMethod.h"
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <cassert>
#include <thread>

class HttpProxy {
private:
  int proxy_fd;
  TcpConnector tcpConnector;
  size_t threadPoolSize;

private:
  std::mutex id_mutex;

  // id of next client
  static size_t next_client_id;

private:
  /*
   * Set client socket fd recv timeout
   */
  void setClientSocketFdRecvTimeout(int client_socket_fd, struct timeval timeout);

  /*
   * Assign client id to client
   * thread safe
   */
  size_t assignClientId();

  /*
   * Parse invalid_argument exception what
   * exception: format: status_code|reason
   */
  void parseInvalidArgExp(const std::string &exception,
                          std::string &status_code, std::string reason);

  /*
   * Send error status to client when exception occur
   */
  void sendErrorStatus(const std::string &status_code,
                       const std::string &reason, HttpConnector &httpConnector);

  /*
   * Execute proxy for the connected client
   * @throw runtime_exception
   */
  void _exec(int client_socket_fd, int client_id);

  /*
   * Call _exec and catch exception
   */
  void exec(int client_socket_fd);

  /*
   * Execute specify http method
   * Only accept GET, POST, CONNECT
   * @throw invalid_exception
   */
  void methodAct(const std::string &method, HttpConnector &httpConnector,
                 Request &request);

public:
  /*
   * Initialize http proxy, start listening
   * @port proxy listening port
   */
  void init(std::string port);

  /*
   * Single thread, for test
   */
  void noneMultiThread();

  /*
   * Multithread
   */
  void multiThread();

  HttpProxy();
};

#endif
