#ifndef __HTTP_METHOD_H__
#define __HTTP_METHOD_H__
#include "HttpConnector.h"

class HttpMethod {
  /*
   * Different http method such as post, get, connect take specific action
   */
  virtual void takeAction(HttpConnector &httpConnector) = 0;
};

#endif
