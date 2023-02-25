#include "GetMethod.h"

void GetMethod::takeAction(HttpConnector &httpConnector, Request &request) {
  // filter invalid field in get method
  filter(request);

  // send request to server
  httpConnector.sendRequest(&request);
  // receive response from server
  Response *response = httpConnector.receiveResponse();
  // send response to client
  try {
    httpConnector.sendResponse(response);
  } catch (std::runtime_error &e) {
    delete response;
    throw;
  }
  delete response;
}

void GetMethod::filter(Request &request) {
  if (request.doesHeaderHave("expect")) {
    _expect();
  }
}

void GetMethod::_expect() {
  throw std::invalid_argument("400|ERROR Expect field is not valid in Get Method");
}
