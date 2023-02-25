#include "GetMethod.h"

void GetMethod::takeAction(HttpConnector &httpConnector, Request &request) {
  // filter invalid field in get method
  // filter(request);

  // // send request to server
  // httpConnector.sendRequest(&request);
  // // receive response from server
  // Response *response = httpConnector.receiveResponse();
  // // send response to client
  // try {
  //   httpConnector.sendResponse(response);
  // } catch (std::runtime_error &e) {
  //   delete response;
  //   throw;
  // }
  // delete response;
}

void GetMethod::filter(Request &request) {
  if (request.doesHeaderHave("expect")) {
    _expect();
  }
}

void GetMethod::_expect() {
  throw std::invalid_argument(
      "400|ERROR Expect field is not valid in Get Method");
}

void GetMethod::takeAction(HttpConnector &httpConnector, Request &request,
                           Cache &cache) {
    Logger::getLogger().proxyLog(0, "NOTE trying put response into cache 0");
  filter(request);
    Logger::getLogger().proxyLog(1, "NOTE trying put response into cache 1");

  Response *response = NULL;
  Node *node = cache.getResponse(request, httpConnector.getClientId());
  if (node != NULL) {
    time_t request_time = time(NULL);
    if (cache.isFresh(request, httpConnector.getClientId(), request_time)) {
      response = &node->response;
    } else {
      cache.revalidation(httpConnector.getClientId(), request, node->response,
                         httpConnector);
      return;
    }
  } else {
    Logger::getLogger().proxyLog(httpConnector.getClientId(), "not in cache");
    // send request to server
    httpConnector.sendRequest(&request);
    // receive response from server
    response = httpConnector.receiveResponse();
    if (cache.isCacheable(*response)) {
      cache.putResponse(request, *response, httpConnector.getClientId());
    }else{
    }
  }

  // send response to client
  try {
    httpConnector.sendResponse(response);
  } catch (std::runtime_error &e) {
    delete response;
    throw;
  }
  delete response;
}
