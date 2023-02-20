#ifndef __CONNECT_METHOD_H__
#define __CONNECT_METHOD_H__
#include "HttpConnector.h"
#include "HttpMethod.h"
#include "Request.h"
#include "Response.h"
#include "ResponseFactory.h"

class ConnectMethod : public HttpMethod {
public:
  /*
   * Connect method
   * @throw runtime_exception
   */
  virtual void takeAction(HttpConnector &httpConnector);
};

#endif
