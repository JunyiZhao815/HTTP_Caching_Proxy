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
  /*
   * Transform http message to string
   */
  virtual std::string message2string() = 0;

    /*
   * Compare if two http message is equal (content is same)
   */
  bool operator==(HttpMessage &httpMessage);

  /*
   * Get field in header
   * @name field name
   * @return field value
   * @throw invalid_argument
   */
  std::string getHeaderField(std::string name);

  /*
   * Check if header contain specific field
   * @name field name
   * @return true/false
   */
  bool doesHeaderHave(std::string name);

  /*
   * Add field to request/response header
   * If exist key, replace the value
   */
  void addHeaderField(const std::string &name, const std::string &value);

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

  std::string getDate();
  
  virtual std::string getFirstLine() = 0;

  virtual ~HttpMessage();

  void removeHeaderField(std::string name);

  /*
   * Calculate the length of whole message, including the start line, header and body
   */
  size_t getMessageLen();
};

#endif
