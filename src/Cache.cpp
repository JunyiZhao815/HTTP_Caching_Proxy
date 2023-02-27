#include "Cache.hpp"
#include "HttpConnector.h"
#include "Logger.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>

Node *Cache::getResponse(Request request, size_t user_id) {
  Logger::getLogger().proxyLog(user_id, "NOTE trying to get response from cache");
  std::string URI = request.getURI();
  return map[URI];
}

void Cache::putResponse(Request request, Response response, size_t user_id) {
  m.lock();
  Logger::getLogger().proxyLog(user_id, "NOTE trying put response into cache");
  Node *pointer = head;
  std::string URI = request.getURI();
  // Logger::getLogger().proxyLog(user_id, "NOTE trying put response into cache 2");

  while (pointer != NULL) {
      // Logger::getLogger().proxyLog(user_id, "NOTE trying put response into cache 2.5");
    if (pointer->URI == URI) {

      //  if(size - (pointer->response).getMessageLen() +
      //  response.getMessageLen() > capacity){
      //   //remove first node of linkedlist, problem: if removed node is
      //   exactly the node we need to update?
      // }
      pointer->response = response;
     m.unlock();

      return;
    }
    pointer = pointer->next;
  }
  // Logger::getLogger().proxyLog(user_id, "NOTE trying put response into cache 3");

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

    // Logger::getLogger().proxyLog(user_id, "NOTE trying put response into cache 4");

  // Delete the first Node if the size excceed the capacity
  if (this->size > capacity) {
    Logger::getLogger().proxyLog(user_id, "NOTE Delete the first node because the current cache size is out of capacity");

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
    // Logger::getLogger().proxyLog(user_id, "NOTE trying put response into cache 5");

  m.unlock();
}

time_t convert_string2timet(std::string time) {
  size_t gmt = time.find("GMT");
  if (gmt != std::string::npos) {
    time.erase(gmt - 1, 4);
  }
  struct tm tm;
  strptime(time.c_str(), "%a, %d %b %Y %H:%M:%S", &tm);
  time_t t = mktime(&tm);
  // std::cout << "The maketime is: " << t << std::endl;
  return t - 18000;
}

time_t get_current_age(Response response) {
  time_t age_value = stoi(response.getAge());
  time_t date_value = convert_string2timet(response.getDate()) + 3600;
  time_t response_time = date_value;
  time_t apparent_age =
      response_time - date_value < 0 ? 0 : response_time - date_value;
  time_t response_delay = response_time - response.getFirstRequestTime();
  // std::cout << "response_time is: " <<response_time << std::endl;
  // std::cout << "response.getFirstRequestTime() is: " <<response.getFirstRequestTime() << std::endl;

  time_t corrected_age_value = age_value + response_delay;
  time_t corrected_initial_age =
      apparent_age > corrected_age_value ? apparent_age : corrected_age_value;
  time_t now = time(NULL); // correct
  time_t resident_time = now - response_time;
  time_t current_age = corrected_initial_age + resident_time;
  return current_age;
}

time_t get_freshness_lifetime(std::string max_age, std::string expires,
                              std::string last_modified,
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
    time_t current_age = get_current_age(response);
    if (current_age > 24 * 60 * 60 && response.getWarning() == "") {
      response.addHeaderField("warn-code", "133");
    }
    return heuristic_lifetime;
  }
  return -1;
}

// finish testing
bool Cache::isFresh(Request request, int user_id) {
  std::string URI = request.getURI();
  Node *response_node = map[URI];
  if (response_node == NULL) {
    std::cerr
        << "Cannot find the response you are trying to check freshing or not"
        << std::endl;
  }
  Response response_old = response_node->response;
  std::string cache_control = response_old.getCacheControl();
  if (cache_control == "") {
    //std::cout << "no cache_control" << std::endl;
    return false;
  } else {
    std::string max_age = response_old.getMaxAge();
    std::string expires = response_old.getExpires();
    std::string last_modified = response_old.getLastModified();
    int freshness_lifetime = get_freshness_lifetime(
        max_age, expires, last_modified, response_old);
    int current_age = get_current_age(response_old);

    // std::cout << "freshness_lifetime is:" << freshness_lifetime << std::endl;
    // std::cout << "current_age is:" << current_age << std::endl;

    if (freshness_lifetime > current_age) {
      //std::cout << "it is fresh" << std::endl;
      Logger::getLogger().proxyLog(user_id, "in cache, valid");
      return true;
    } else {
      //std::cout << "it is not fresh" << std::endl;
      print_expire(user_id, response_old, "in cache, but expired at ");
      Logger::getLogger().proxyLog(user_id,"in cache, requires validation");
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
    time_t expire_age = date_age + max_age_int + 3600;
    struct tm *exp = gmtime(&expire_age);
    std::string expire_time_act = asctime(exp); 
    expire_time_act.pop_back();
    Logger::getLogger().proxyLog(user_id, words + expire_time_act);
  } else if (expires != "") {
    time_t expire_time = convert_string2timet(expires);
    struct tm *exp = gmtime(&expire_time);
    std::string expire_time_act = asctime(exp); 
    expire_time_act.pop_back();
    Logger::getLogger().proxyLog(user_id, words + expire_time_act);
  } else if (last_modified != "") {
    time_t date_age = convert_string2timet(date) + 3600;
    time_t last_modified_age = convert_string2timet(last_modified);
    time_t exp_age = time(NULL) + difftime(date_age, last_modified_age) / 10;
    struct tm *exp = gmtime(&exp_age);
    std::string expire_time_act = asctime(exp); 
    expire_time_act.pop_back();
    Logger::getLogger().proxyLog(user_id, words + expire_time_act);
  }
}

void Cache::revalidation(int user_id, Request request, Response response, HttpConnector& httpConnector) {
  std::string etag = response.getEtag();
  std::string lastModified = response.getLastModified();
  // 1. Sending a Validation Request
  if (etag != "") {
    //std::cout << "etag" << std::endl;
    check_validation(request, response, user_id, "If-None-Match", etag, httpConnector);
  } else if (lastModified != "") {
    //std::cout << "last modified" << std::endl;
    check_validation(request, response, user_id, "If-Modified-Since",
                     lastModified, httpConnector);
  } else {
    //std::cout << "else, resend" << std::endl;
    check_validation(request, response, user_id, "", "", httpConnector);
  }
  Logger::getLogger().proxyLog(user_id,"NOTE finish revalidation");

}

void Cache::check_validation(Request request, Response response, int user_id,
                             std::string tag, std::string value, HttpConnector &httpConnector) {
  if (tag == "If-None-Match") {
    Logger::getLogger().proxyLog(user_id, "NOTE ETag: " + value);
  } else if (tag == "If-Modified-Since") {
    Logger::getLogger().proxyLog(user_id, "NOTE Last-Modified: " + value);
  } else {
    Logger::getLogger().proxyLog(user_id, "WARNING send again");
  }

  Request newRequest = request;
  if (tag != "") {
    newRequest.addHeaderField(tag, value);
  }
  //std::cout << newRequest << std::endl;

  // send the request to the server
  Logger::getLogger().proxyLog(httpConnector.getClientId(), "Requesting \"" + request.getFirstLine() + "\" from " + newRequest.getHost().first);
  httpConnector.sendRequest(&newRequest);

  Response *newResponse =
      httpConnector.receiveResponse(); // receive the new response from the server, and check
  Logger::getLogger().proxyLog(httpConnector.getClientId(), "Received \"" + newResponse->getFirstLine() + "\" from " + newRequest.getHost().first);
  
  //std::cout <<"code is: "<< newResponse->getStatusCode() << std::endl;
  if (newResponse->getStatusCode() == "304") {
    // respond from cache
    //std::cout << "code = 304" << std::endl;
    Response response_to_send = response;
    // response_to_send.setReason(newResponse->getReason());
    // response_to_send.setStatus(newResponse->getStatus());
    // respond_to_client(response_to_send, user_id, httpConnector);
    httpConnector.sendMessage(response, true);
    std::string first = newResponse->getFirstLine();
    Logger::getLogger().proxyLog(user_id, "Responding " + first);
    // respond_to_client(response_to_send, user_id, httpConnector);

  } else if (newResponse->getStatusCode() == "200") {
    //std::cout << "code = 200" << std::endl;
    // If the new response is cacheable
    if (newResponse->getCacheable() == "yes") {
      // Update response
      putResponse(request, *newResponse, user_id);
      if (newResponse->getCacheControl() != "") {
        if (newResponse->getCacheControl().find("must-revalidate") !=
            std::string::npos) {
          Logger::getLogger().proxyLog(
              user_id, "cached, but requires re-validation" + value);
        } else {
          print_expire(user_id, response, "cached, expires at ");

        }
      }
    } else {
      if (newResponse->getCacheable() == "") {
        Logger::getLogger().proxyLog(
            user_id, "not cacheable because cache-control tag is empty");
      } else {
        Logger::getLogger().proxyLog(user_id, "not cacheable because " +
                                                  newResponse->getCacheable());
      }
    }
    respond_to_client(*newResponse, user_id, httpConnector);
  }

}

void Cache::respond_to_client(Response response, int user_id, HttpConnector& httpConnector) {
  httpConnector.sendMessage(response, true);
  std::string first = response.getFirstLine();
  Logger::getLogger().proxyLog(user_id, "Responding " + first);
}

bool Cache::isCacheable(Response response) {
  if (response.getCacheable() == "yes") {
    return true;
  } else {
    return false;
  }
}
std::string Cache::getCurrUTCtime() {
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t utc_time = std::chrono::system_clock::to_time_t(now);
  std::tm utc_tm = *std::gmtime(&utc_time);
  std::stringstream ss;
  ss << std::put_time(&utc_tm, "%a %b %d %T %Y");
  return ss.str();
}

void Cache::log_cacheable(Response& response, int user_id){
  if (response.getCacheable() == "yes") {
      if (response.getCacheControl() != "") {
        if (response.getCacheControl().find("must-revalidate") != std::string::npos) {
          //std:: cout << "it is: "<<response.getCacheControl().find("must-revalidate") << std::endl;
          Logger::getLogger().proxyLog(user_id, "cached, but requires re-validation");
        } else {
          print_expire(user_id, response, "cached, expires at ");
        }
      }
    } else {
      if (response.getCacheable() == "") {
        Logger::getLogger().proxyLog(
            user_id, "not cacheable because cache-control tag is empty");
      } else {
        Logger::getLogger().proxyLog(user_id, "not cacheable because " +
                                                  response.getCacheable());
      }
    }
}