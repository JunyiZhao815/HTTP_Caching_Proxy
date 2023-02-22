#include "GetMethod.h"

void GetMethod::takeAction(HttpConnector &httpConnector, Request &request) {
  // filter invalid field in get method
  filter(request);
  // send request to server
  httpConnector.sendRequest(&request);
  // receive response from server
  Response *response = httpConnector.receiveResponse();
  // send response to client
  httpConnector.sendResponse(response);
  delete response;
}

void GetMethod::filter(Request &request) {
  if (request.doesHeaderHave("expect")) {
    _expect();
  }
}

void GetMethod::_expect() { throw std::invalid_argument("400|Except field is not valid in Get Method"); }
