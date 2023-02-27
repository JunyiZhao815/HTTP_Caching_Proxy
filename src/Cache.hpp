#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include "HttpConnector.h"
#include "Logger.h"
#include "Request.h"
#include "Response.h"
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

class Node {
public:
  std::string URI;
  Response response;
  Node *prev = NULL;
  Node *next = NULL;
  Node(std::string URI, Response response) : URI(URI), response(response) {}
};

// Must define it after httpConnector, otherwise the httpConnector inside of
// cache class will be empty.
class Cache {

private:
  void print_expire(int user_id, Response response, std::string words);
public:
  void log_cacheable(Response& response, int user_id);

public:
  // Here I use First In First Out(FIFO) to implement the replacement policy
  std::mutex m;
  std::map<std::string, Node *> map;
  size_t capacity;
  size_t size;
  Node *head;
  Node *tail;
  Cache(size_t capacity, size_t size) : capacity(capacity), size(size) {}

  /*
  It is used to get the response from map
  If we get the response successfully, return the node*, else return null;
  */
  Node *getResponse(Request request, size_t user_id);

  /*
  Put the pair<URI, node*> into the map
  Also append the linkedlist.
  If linkedlist excceeds the capacity, removes the head node.
  */
  void putResponse(Request request, Response response, size_t user_id);

  /*
  Check if the response is fresh, return true if fresh, else false
  @param time_t request_time is the current value of the clock at the host at
  the time the request resulting in the stored response was made. Which means
  the time of request that the stored response is made. you can use time_t
  request_time = time(NULL) to get.

  If the response is fresh, we directly use it, otherwise, we go to revalidation
  part.
  */
  bool isFresh(Request request, int user_id);

  /*
  This function revalidation the request and response.
  The revalidation process:
  send the request to the server,
  if the returned response's status code in 200, then store the response and
  give the response to the client, else if 304, give the previous response to
  the client.

  @param: the request is the current request that the client made
  @param: the response is the stored response in the map
  */
  void revalidation(int user_id, Request request, Response response, HttpConnector& httpConnector);

  /*
  It is helper function of revalidation(), do not use it in other class.
  */
  void check_validation(Request request, Response response, int user_id,
                        std::string tag, std::string value,
                        HttpConnector &httpConnector);

  /*
  send the response to the client, do not use it because it is already inside of
  revalidation();
  */
  void respond_to_client(Response response, int user_id, HttpConnector& httpConnector);

  /*
  check if the current response is cacheable
  */
  bool isCacheable(Response response);


  std::string getCurrUTCtime();

};

#endif