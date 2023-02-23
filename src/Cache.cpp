#include "Cache.hpp"
#include "HttpConnector.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <Logger.h>


Node* Cache::getResponse(Request request) {
  std::string URI = request.getURI();
  return map[URI];
}

void Cache::putResponse(Request request, Response response) {
  m.lock();
  Node *pointer = head;
  std::string URI = request.getURI();
  while (pointer != NULL) {
    if (pointer->URI == URI) {
      pointer->response = response;
      return;
    }
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

int get_freshness_lifetime(std::string max_age, std::string expires,
                           std::string last_modified, Request request,
                           Response response) {
  if (max_age != "") {
    return convert_string2timet(max_age);
  } else if (expires != "") {
    time_t expire_time = convert_string2timet(expires);
    time_t response_time = convert_string2timet(response.getDate());
    return expire_time - response_time;
  } else if (last_modified != "") {
    time_t last_modified_time = convert_string2timet(last_modified);
    time_t date = convert_string2timet(response.getDate());
    time_t heuristic_lifetime = difftime(date, last_modified_time) / 10;
    time_t current_age = get_current_age(request, response);
    if (current_age > 24 * 60 * 60 && response.getWarning() == "") {
      response.addHeaderField("warn-code", "133");
    }
    return heuristic_lifetime;
  }
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



time_t get_current_age(Request request, Response response) {
  time_t age_value = stoi(response.getAge());
  time_t date_value = convert_string2timet(response.getDate());
  time_t now = time(NULL);
  time_t request_time = convert_string2timet(request.getDate());
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



bool Cache::isFresh(Request request, Response response, int user_id) {
  Node* response_node = this->getResponse(request);
  if (response_node == NULL) {
    std::cerr
        << "Cannot find the response you are trying to check freshing or not"
        << std::endl;
  }
  Response response_old = response_node->response;
  std::string cache_control = response_old.getCacheControl();
  if (cache_control == "") {
    return false;
  } else {
    std::string max_age = response_old.getMaxAge();
    std::string expires = response_old.getExpires();
    std::string last_modified = response_old.getLastModified();
    int freshness_lifetime = get_freshness_lifetime(
        max_age, expires, last_modified, request, response);
    int current_age = get_current_age(request, response);
    if (freshness_lifetime > current_age) {
      std::cout << "it is fresh" << std::endl;
      return true;
    } else {
      std::cout << "it is not fresh" << std::endl;
      return false;
    }
  }
}


void print_expire(int user_id, Response response){
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
        Logger::getLogger().proxyLog(user_id, ": cached, expires at " + std::string(expire_time_act));
    } else if (expires != "") {
        time_t expire_time = convert_string2timet(expires);
        struct tm *exp = gmtime(&expire_time);
        const char*expire_time_act = asctime(exp);
        Logger::getLogger().proxyLog(user_id, ": cached, expires at " + std::string(expire_time_act));
    } else if (last_modified != "") {
        time_t date_age = convert_string2timet(date);
        time_t last_modified_age = convert_string2timet(last_modified);
        time_t exp_age = time(NULL) + difftime(date_age, last_modified_age) / 10;
        struct tm *exp = gmtime(&exp_age);
        const char *expire_time_act = asctime(exp);
        Logger::getLogger().proxyLog(user_id, ": cached, expires at " + std::string(expire_time_act));
    }
}



void Cache::revalidation(int user_id, Response response, Request request) {
  std::string etag = response.getEtag();
  std::string lastModified = response.getLastModified();
  // 1. Sending a Validation Request
  TcpConnector tcp = TcpConnector();
  if (etag != "") {
    check_validation(request, response, user_id, "If-None-Match", etag);
  } else if (lastModified != "") {
    check_validation(request, response, user_id, "If-Modified-Since", lastModified);
  } else {
    check_validation(request, response, user_id, "", "");
  }
}

void Cache::check_validation(Request request, Response response, int user_id, std::string tag, std::string value) {
  if(tag == "If-None-Match"){
    Logger::getLogger().proxyLog(user_id, ": NOTE ETag: " + value);
  }else if(tag == "If-Modified-Since"){
    Logger::getLogger().proxyLog(user_id, ": NOTE Last-Modified: " + value);
  }else{
    Logger::getLogger().log("send again");
  }
  Request newRequest = request;
  if(tag != ""){
    newRequest.addHeaderField(tag, value);
  }
  httpConnector.sendMessage(newRequest, false);
  Response* newResponse = httpConnector.receiveResponse();

  if (newResponse->getStatusCode() == "304") {
    // respond from cache
    std::cout << "code = 304" << std::endl;
    respond_to_client(response, user_id);

  } else if (newResponse->getStatusCode() == "200") {
    std::cout << "cod = 200" << std::endl;
    //If the new response is cacheable
    if(newResponse->getCacheable() == "yes"){
      //Update response
      putResponse(request, *newResponse);
      if(newResponse->getCacheControl() != ""){
        if (newResponse->getCacheControl().find("must-revalidate")!=-1){
            Logger::getLogger().proxyLog(user_id, ": cached, but requires re-validation" + value);
        }else{
            print_expire(user_id, response);
        }
      }
    }else{
        if (newResponse->getCacheable()==""){
          Logger::getLogger().proxyLog(user_id, ": not cacheable because cache control not specified");
        }else{
          Logger::getLogger().proxyLog(user_id, ": not cacheable because "+ newResponse->getCacheable());
        }
    }
    respond_to_client(*newResponse, user_id);
  }
}

void Cache::respond_to_client(Response response, int user_id){
    httpConnector.sendMessage(response, true);
    std::string first = response.getFirstLine();
    Logger::getLogger().proxyLog(user_id, ": Responding "  + first);
}

bool Cache::isCacheable(Response response){
  if(response.getCacheable() == "yes"){
    return true;
  }else{
    return false;
  }
}

