#include "ConnectMethod.h"

void ConnectMethod::takeAction(HttpConnector &httpConnector) {
  ResponseFactory factory;

  // send back 200 ok to client
  Response response = factory.create_200_ok_response();
  httpConnector.sendResponse(&response);

  // create tunnel and send data between client and server
  httpConnector.tunnelTransport();
}
