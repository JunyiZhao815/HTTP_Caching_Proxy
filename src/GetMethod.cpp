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
  //   Logger::getLogger().proxyLog(0, "NOTE trying put response into cache 0");
  // filter(request);
  //   Logger::getLogger().proxyLog(1, "NOTE trying put response into cache 1");

  Response *response = NULL;
            std::cout << "0" << std:: endl;

  Node *node = cache.getResponse(request, httpConnector.getClientId());
  if (node != NULL) {
    node->response.setFirstRequestTime(time(0));
    if (cache.isFresh(request, httpConnector.getClientId())) {
      std::cout << "1" << std:: endl;
      response = &(node->response);
      std::cout << "2" << std:: endl;
    } else {
      std::cout << "7" << std:: endl;

      cache.revalidation(httpConnector.getClientId(), request, node->response,
                         httpConnector);
                               std::cout << "8" << std:: endl;

      return;
    }
  } else {
    Logger::getLogger().proxyLog(httpConnector.getClientId(), "not in cache");
    // send request to server
      std::cout << "9" << std:: endl;

    httpConnector.sendRequest(&request);

    // receive response from server
    response = httpConnector.receiveResponse();
    if (cache.isCacheable(*response)) {
                std::cout << "10" << std:: endl;

      cache.putResponse(request, *response, httpConnector.getClientId());
                std::cout << "11" << std:: endl;

    }
  }

  // send response to client
  try {
          std::cout << "3" << std:: endl;

    httpConnector.sendResponse(response);
          std::cout << "4" << std:: endl;

  } catch (std::runtime_error &e) {
    delete response;
    throw;
  }
        std::cout << "5" << std:: endl;

  //delete response;
        std::cout << "6" << std:: endl;

}
