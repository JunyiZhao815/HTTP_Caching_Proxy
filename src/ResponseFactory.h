#ifndef __RESPONSE_FACTORY_H__
#define __RESPONSE_FACTORY_H__
#include "Response.h"

class ResponseFactory {
public:
  /*
   * Create response with 200 ok, no body
   */
  Response create_200_ok_response();
};

#endif
