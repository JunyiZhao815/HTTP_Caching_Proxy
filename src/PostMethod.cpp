#include "PostMethod.h"

void PostMethod::takeAction(HttpConnector &httpConnector, Request &request) {
  // filter invalid field in post method
  filter(request);
  // send request to server
  Logger::getLogger().proxyLog(httpConnector.getClientId(), "Requesting \"" + request.getFirstLine() + "\" from " + request.getHost().first);
  httpConnector.sendRequest(&request);
  // receive response from server
  Response *response = httpConnector.receiveResponse();
  Logger::getLogger().proxyLog(httpConnector.getClientId(), "Received \"" + response->getFirstLine() + "\" from " + request.getHost().first);
  // send response to client
  try{
    httpConnector.sendResponse(response);
    std::string logmsg = "Responding \"" + response->getFirstLine() + "\"";
    Logger::getLogger().proxyLog(httpConnector.getClientId(), logmsg);
  }catch(std::runtime_error& e){
    delete response;
    throw;
  }
  delete response;
}

void PostMethod::filter(Request &request) {}
