#include "HttpMethod.h"

HttpMethod::~HttpMethod(){}

void HttpMethod::takeAction(HttpConnector &httpConnector, Request& request, Cache& cache){
    takeAction(httpConnector, request);
}