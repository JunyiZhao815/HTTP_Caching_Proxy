#ifndef __HTTP_PROXY_H__
#define __HTTP_PROXY_H__
#include "ConnectMethod.h"
#include "GetMethod.h"
#include "HttpConnector.h"
#include "PostMethod.h"

class HttpProxy {
private:
  int proxy_fd;
  TcpConnector tcpConnector;

private:
  /*
   * Send error status to client when exception occur
   */
  void sendErrorStatus(std::string status_code, std::string reason,
                       HttpConnector &httpConnector);

  /*
   * Execute proxy for the connected client
   * @throw runtime_exception
   */
  void _exec(int client_socket_fd);

  /*
   * Call _exec and catch exception
   */
  void exec(int client_socket_fd);

  /*
   * Execute specify http method
   */
  void methodAct(const std::string &method, HttpConnector &httpConnector,
                 Request &request);

  /*
   * Filt unaccept http method
   * currently: GET, POST, CONNECT
   * @throw argument_invalid(405)
   */
  void filtHttpMethod(const std::string method);

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
};

#endif
