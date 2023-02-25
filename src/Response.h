#ifndef __RESPONSE_H__
#define __RESPONSE_H__
#include "HttpMessage.h"

class Response : public HttpMessage {
private:
  std::string status_code;
  std::string reason;

private:
  std::string getStartLine() const;

public:
  Response(const unsigned int version, const std::string &status_code,
           const std::string &reason,
           std::map<std::string, std::string> &header, std::string &body);

  friend std::ostream &operator<<(std::ostream &out, const Response response);

  /*
   * Transform request to string
   */
  std::string message2string() override;

  /*
   * Get Expires (expires) field from header
   */
  std::string getExpires();

  /*
   * Get Last-Modified (last-modified) field from header
   */
  std::string getLastModified();

  std::string getStatusCode();

  std::string getEtag();

  std::string getTransferEncoding();

  std::string getAge();

  std::string getWarning();

  std::string getCacheable();

  std::string getFirstLine() override;

  /*
   * Calculate content length, without calculating the header and first line
   */
  size_t getContentLen();

  void setStatus(std::string status);

  void setReason(std::string reason);

  std::string getStatus();

  std::string getReason();
};
#endif
