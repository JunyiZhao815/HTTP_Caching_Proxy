#ifndef __RESPONSE_FACTORY_H__
#define __RESPONSE_FACTORY_H__
#include "Response.h"

class ResponseFactory {
public:
  /*
   * Create response with 200 ok, no body
   */
  Response create_200_ok_response();

  /*
   * Create response with 4xx, reason, no body
   */
  Response create_4xx_response(const std::string &status_code,
                               const std::string &reason);
};

#endif
