#ifndef __HTTP_MESSAGE_H__
#define __HTTP_MESSAGE_H__
#include <iostream>
#include <map>
#include <sstream>

class HttpMessage {
protected:
  unsigned int version;
  std::map<std::string, std::string> header;
  std::string body;

protected:
  /*
   * Check if header exist key as name
   * @throw invalid_argument
   */
  void checkHasField(const std::string &name) const;

public:
  HttpMessage(const unsigned int version,
              std::map<std::string, std::string> &header, std::string &body);

  /*
   * Transform header to string
   */
  std::string header2string() const;

  std::string getVersion() const;

  /*
   * Get Cache-Control (cache-control) from header
   * @throw invalid_argument
   */
  std::string getCacheControl();

  /*
   * Get max-age from header
   * @throw invalid_argument
   */
  std::string getMaxAge();

  /*
   * Transform http message to string
   */
  virtual std::string message2string() = 0;

  /*
   * Compare if two http message is equal (content is same)
   */
  bool operator==(HttpMessage &httpMessage);

  virtual ~HttpMessage();
};

#endif
