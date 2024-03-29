#ifndef __GET_METHOD_H__
#define __GET_METHOD_H__
#include "HttpMethod.h"

class GetMethod : public HttpMethod {
public:
  virtual void takeAction(HttpConnector &httpConnector, Request &request) override;

  virtual void takeAction(HttpConnector &httpConnector, Request &request,
                          Cache &cache) override;

private:
  /*
   * If meet expect field in get method, return 400
   * @throw invalid_argument(400)
   */
  void _expect();

  /*
   * Check if exist invalid field
   * @throw invalid_argument
   */
  void filter(Request &request);
};

#endif
