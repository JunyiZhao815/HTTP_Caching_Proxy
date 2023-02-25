#include "HttpProxy.h"

int main() {
  Cache cache(100, 0);
  HttpProxy httpProxy(cache);
  httpProxy.init("4000");
  // httpProxy.multiThread();
  httpProxy.noneMultiThread();
}
