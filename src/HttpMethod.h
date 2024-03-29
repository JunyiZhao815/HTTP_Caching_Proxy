#ifndef __HTTP_METHOD_H__
#define __HTTP_METHOD_H__
#include "HttpConnector.h"
#include "Logger.h"
#include "Cache.hpp"

class HttpMethod {
public:
  /*
   * Different http method such as post, get, connect take specific action
   */
  virtual void takeAction(HttpConnector &httpConnector, Request& request) = 0;

  virtual void takeAction(HttpConnector &httpConnector, Request& request, Cache& cache);

  virtual ~HttpMethod();
};

#endif
