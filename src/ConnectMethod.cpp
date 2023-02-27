#include "ConnectMethod.h"

void ConnectMethod::takeAction(HttpConnector &httpConnector, Request& request) {
  ResponseFactory factory;

  // send back 200 ok to client
  Response response = factory.create_200_ok_response();
  httpConnector.sendResponse(&response);
  std::string logmsg = "Responding \"" + response.getFirstLine() + "\"";
  Logger::getLogger().proxyLog(httpConnector.getClientId(), logmsg);

  // create tunnel and send data between client and server
  httpConnector.tunnelTransport();

  // tell that tunnel closed
  Logger::getLogger().proxyLog(httpConnector.getClientId(), "Tunnel closed");
}
