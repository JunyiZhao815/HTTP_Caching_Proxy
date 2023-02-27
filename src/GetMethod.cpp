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
  Response *response = NULL;
  Node *node = cache.getResponse(request, httpConnector.getClientId());
  if (node != NULL) {
    std::string curTime = cache.getCurrUTCtime();
    struct tm tm;
    strptime(curTime.c_str(), "%a %b %d %T %Y", &tm);
    time_t t = mktime(&tm);
    node->response.setFirstRequestTime(t - 14400);
    if (cache.isFresh(request, httpConnector.getClientId())) {
      response = &(node->response);
    } else {
      cache.revalidation(httpConnector.getClientId(), request, node->response,
                         httpConnector);
      return;
    }
  } else {
    Logger::getLogger().proxyLog(httpConnector.getClientId(), "not in cache");
    // send request to server
    Logger::getLogger().proxyLog(httpConnector.getClientId(), "Requesting \"" + request.getFirstLine() + "\" from " + request.getHost().first);
    httpConnector.sendRequest(&request);

    // receive response from server
    response = httpConnector.receiveResponse();
    Logger::getLogger().proxyLog(httpConnector.getClientId(), "Received \"" + response->getFirstLine() + "\" from " + request.getHost().first);
    if (cache.isCacheable(*response)) {
      cache.putResponse(request, *response, httpConnector.getClientId());
    }
    cache.log_cacheable(*response, httpConnector.getClientId());
  }

  // send response to client
  try {
    httpConnector.sendResponse(response);
    std::string logmsg = "Responding \"" + response->getFirstLine() + "\"";
    Logger::getLogger().proxyLog(httpConnector.getClientId(), logmsg);
  } catch (std::runtime_error &e) {
    delete response;
    throw;
  }
  // delete response;
}
