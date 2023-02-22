#include "HttpConnector.h"
#include "Request.h"
#include "Response.h"
#include <cstdlib>
#include <iostream>
#include <iostream>
#include <map>
#include <vector>

class Node {
public:
  std::string URI;
  Response response;
  Node *prev;
  Node *next;
  Node(std::string URI, Response response) : URI(URI), response(response) {}
};
class Cache {
private:
  HttpConnector httpConnector;

public:
  // Here I use First In First Out(FIFO) to implement the replacement policy
  std::map<std::string, Node *> map;
  size_t capacity;
  size_t size;
  Node *head;
  Node *tail;
  Cache(HttpConnector &httpConnector, size_t capacity, size_t size)
      : httpConnector(httpConnector), capacity(capacity), size(size) {}
  void usingCache(Request request, int user_id);
  Node* getResponse(Request request);
  void putResponse(Request request, Response response);
  bool isFresh(Request request, Response response, std::ostream &out,
               int user_id);
  void revalidation(int user_id, Response response, Request request);
  void check_validation(Request request, Response response, int user_id, std::string tag, std::string value);
  void respond_to_client(Response response, int user_id);
  bool isCacheable(Response response);
};

