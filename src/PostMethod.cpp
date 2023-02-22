#include "PostMethod.h"

void PostMethod::takeAction(HttpConnector &httpConnector, Request &request) {
  // filter invalid field in post method
  filter(request);
  // send request to server
  httpConnector.sendRequest(&request);
  // receive response from server
  Response *response = httpConnector.receiveResponse();
  // send response to client
  try{
    httpConnector.sendResponse(response);
  }catch(std::runtime_error& e){
    delete response;
    throw;
  }
  delete response;
}

void PostMethod::filter(Request &request) {}
