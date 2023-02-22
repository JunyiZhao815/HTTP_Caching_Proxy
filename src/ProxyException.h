#ifndef __PROXY_EXCEPTION_H__
#define __PROXY_EXCEPTION_H__
#include <exception>
#include <string>

class ProxyException : public std::exception {
private:
  const std::string status_code;
  const std::string description;

public:
  const char *what() const noexcept override;

  // ProxyException(const std::string &status_code,
  //                const std::string &description);
};

#endif
