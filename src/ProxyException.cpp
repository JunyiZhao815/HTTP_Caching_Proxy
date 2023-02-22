#include "ProxyException.h"

const char *ProxyException::what() const {
  return "exception";
}
