#ifndef __POST_METHOD_H__
#define __POST_METHOD_H__
#include "HttpMethod.h"

class PostMethod : public HttpMethod {
public:
  virtual void takeAction(HttpConnector &httpConnector, Request &request);

private:
  void filter(Request &request);
};

#endif
