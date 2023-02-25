#include "Cache.hpp"
#include "HttpConnector.h"
#include "Logger.h"
#include <algorithm>
#include <fstream>
#include <iostream>

Node *Cache::getResponse(Request request) {
  std::string URI = request.getURI();
  return map[URI];
}

void Cache::putResponse(Request request, Response response) {
  m.lock();
  Node *pointer = head;
  std::string URI = request.getURI();
  while (pointer != NULL) {
    if (pointer->URI == URI) {
      //  if(size - (pointer->response).getMessageLen() +
      //  response.getMessageLen() > capacity){
      //   //remove first node of linkedlist, problem: if removed node is
      //   exactly the node we need to update?
      // }
      pointer->response = response;
      return;
    }
    pointer = pointer->next;
  }
  // Add Node to the end of linkedlist
  Node *temp = new Node(URI, response);
  if (size == 0) {
    head = temp;
    tail = temp;
  } else {
    tail->next = temp;
    tail->next->prev = tail;
    tail = tail->next;
  }
  map[URI] = temp;
  ++size;
  // Delete the first Node if the size excceed the capacity
  if (this->size > capacity) {
    Node *first = head;
    map[first->URI] = NULL;
    head = head->next;
    if (head == NULL) {
      tail = NULL;
    } else {
      head->prev = NULL;
    }
    --size;
  }
  m.unlock();
}

time_t convert_string2timet(std::string time) {
  size_t gmt = time.find("GMT");
  if (gmt != std::string::npos) {
    time.erase(gmt - 1, 4);
  }
  struct tm tm;
  strptime(time.c_str(), "%a, %d %b %Y %H:%M:%S", &tm);
  return mktime(&tm) - 18000;
}

time_t get_current_age(time_t request_time, Response response) {
  time_t age_value = stoi(response.getAge());
  time_t date_value = convert_string2timet(response.getDate());
  time_t now = time(NULL);
  time_t response_time = convert_string2timet(response.getDate());
  time_t apparent_age =
      response_time - date_value < 0 ? 0 : response_time - date_value;
  time_t response_delay = response_time - request_time;
  time_t corrected_age_value = age_value + response_delay;
  time_t corrected_initial_age =
      apparent_age > corrected_age_value ? apparent_age : corrected_age_value;
  time_t resident_time = now - response_time;
  time_t current_age = corrected_initial_age + resident_time;
  return current_age;
}

time_t get_freshness_lifetime(std::string max_age, std::string expires,
                              std::string last_modified, time_t request_time,
                              Response response) {
  if (max_age != "") {
    time_t a = stoi(max_age);
    return a;
  } else if (expires != "") {
    time_t expire_time = convert_string2timet(expires);
    time_t response_time = convert_string2timet(response.getDate());
    return expire_time - response_time;
  } else if (last_modified != "") {
    time_t last_modified_time = convert_string2timet(last_modified);
    time_t date = convert_string2timet(response.getDate());
    time_t heuristic_lifetime = difftime(date, last_modified_time) / 10;
    time_t current_age = get_current_age(request_time, response);
    if (current_age > 24 * 60 * 60 && response.getWarning() == "") {
      response.addHeaderField("warn-code", "133");
    }
    return heuristic_lifetime;
  }
  return -1;
}

// finish testing
bool Cache::isFresh(Request request, int user_id, time_t request_time) {
  Node *response_node = this->getResponse(request);
  if (response_node == NULL) {
    std::cerr
        << "Cannot find the response you are trying to check freshing or not"
        << std::endl;
  }
  Response response_old = response_node->response;
  std::string cache_control = response_old.getCacheControl();
  if (cache_control == "") {
    std::cout << "no cache_control" << std::endl;
    return false;
  } else {
    std::string max_age = response_old.getMaxAge();
    std::string expires = response_old.getExpires();
    std::string last_modified = response_old.getLastModified();
    int freshness_lifetime = get_freshness_lifetime(
        max_age, expires, last_modified, request_time, response_old);
    int current_age = get_current_age(request_time, response_old);

    std::cout << "freshness_lifetime is:" << freshness_lifetime << std::endl;
    std::cout << "current_age is:" << current_age << std::endl;

    if (freshness_lifetime > current_age) {
      std::cout << "it is fresh" << std::endl;
      print_expire(user_id, response_old, ": in cache, but expired at ");
      return true;
    } else {
      std::cout << "it is not fresh" << std::endl;
      print_expire(user_id, response_old, ": in cache, requires validation");
      return false;
    }
  }
}

void Cache::print_expire(int user_id, Response response, std::string words) {
  std::string max_age = response.getMaxAge();
  std::string expires = response.getExpires();
  std::string last_modified = response.getLastModified();
  std::string date = response.getDate();
  if (max_age != "") {
    time_t max_age_int = stoi(max_age);
    time_t date_age = convert_string2timet(date);
    time_t expire_age = date_age + max_age_int;
    struct tm *exp = gmtime(&expire_age);
    const char *expire_time_act = asctime(exp);
    Logger::getLogger().proxyLog(user_id, words + std::string(expire_time_act));
  } else if (expires != "") {
    time_t expire_time = convert_string2timet(expires);
    struct tm *exp = gmtime(&expire_time);
    const char *expire_time_act = asctime(exp);
    Logger::getLogger().proxyLog(user_id, words + std::string(expire_time_act));
  } else if (last_modified != "") {
    time_t date_age = convert_string2timet(date);
    time_t last_modified_age = convert_string2timet(last_modified);
    time_t exp_age = time(NULL) + difftime(date_age, last_modified_age) / 10;
    struct tm *exp = gmtime(&exp_age);
    const char *expire_time_act = asctime(exp);
    Logger::getLogger().proxyLog(user_id, words + std::string(expire_time_act));
  }
}

void Cache::revalidation(int user_id, Request request, Response response, HttpConnector& httpConnector) {
  std::string etag = response.getEtag();
  std::string lastModified = response.getLastModified();
  // 1. Sending a Validation Request
  if (etag != "") {
    std::cout << "etag" << std::endl;
    check_validation(request, response, user_id, "If-None-Match", etag, httpConnector);
  } else if (lastModified != "") {
    std::cout << "last modified" << std::endl;
    check_validation(request, response, user_id, "If-Modified-Since",
                     lastModified, httpConnector);
  } else {
    std::cout << " else, resend" << std::endl;
    check_validation(request, response, user_id, "", "", httpConnector);
  }
}

void Cache::check_validation(Request request, Response response, int user_id,
                             std::string tag, std::string value, HttpConnector &httpConnector) {
  if (tag == "If-None-Match") {
    Logger::getLogger().proxyLog(user_id, ": NOTE ETag: " + value);
  } else if (tag == "If-Modified-Since") {
    Logger::getLogger().proxyLog(user_id, ":E Last-Modified: " + value);
  } else {
    Logger::getLogger().proxyLog(user_id, "send again");
  }

  Request newRequest = request;
  if (tag != "") {
    newRequest.addHeaderField(tag, value);
  }
  std::cout << newRequest << std::endl;

  // send the request to the server
  httpConnector.sendRequest(&newRequest);

  Response *newResponse =
      httpConnector.receiveResponse(); // receive the new response from the
                                       // server, and check
  std::cout << newResponse->getStatusCode() << std::endl;
  if (newResponse->getStatusCode() == "304") {
    // respond from cache
    std::cout << "code = 304" << std::endl;
    Logger::getLogger().proxyLog(user_id, ": NOTE the status code is 304");
    respond_to_client(response, user_id, httpConnector);
  } else if (newResponse->getStatusCode() == "200") {
    std::cout << "cod = 200" << std::endl;
    // If the new response is cacheable
    if (newResponse->getCacheable() == "yes") {
      // Update response
      putResponse(request, *newResponse);
      if (newResponse->getCacheControl() != "") {
        if (newResponse->getCacheControl().find("must-revalidate") !=
            (long)(unsigned)-1) {
          Logger::getLogger().proxyLog(
              user_id, ": cached, but requires re-validation" + value);
        } else {
          print_expire(user_id, response, ": cached, expires at ");
        }
      }
    } else {
      if (newResponse->getCacheable() == "") {
        Logger::getLogger().proxyLog(
            user_id, ": not cacheable because cache-control tag is empty");
      } else {
        Logger::getLogger().proxyLog(user_id, ": not cacheable because " +
                                                  newResponse->getCacheable());
      }
    }
    respond_to_client(*newResponse, user_id, httpConnector);
  }
}

void Cache::respond_to_client(Response response, int user_id, HttpConnector& httpConnector) {
  httpConnector.sendMessage(response, true);
  std::string first = response.getFirstLine();
  Logger::getLogger().proxyLog(user_id, ": Responding " + first);
}

bool Cache::isCacheable(Response response) {
  if (response.getCacheable() == "yes") {
    return true;
  } else {
    return false;
  }
}
