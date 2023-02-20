#ifndef __HTTP_MESSAGE_H__
#define __HTTP_MESSAGE_H__
#include <iostream>
#include <map>
#include <sstream>

class HttpMessage {
protected:
  const unsigned int version;
  std::map<std::string, std::string> header;
  std::string body;

public:
  HttpMessage(const unsigned int version,
              std::map<std::string, std::string> &header, std::string &body);

  /*
   * Transform header to string
   */
  std::string header2string() const;

  std::string getVersion() const;

  //For both request and response
  std::string getCacheControl() const;

  //For both request and response
  std::string getMaxAge() const;

  //Only in response
  std::string getExpires() const;

  //Only in response
  std::string getLastModified() const;

  //For response
  std::string getDate() const;
};

#endif